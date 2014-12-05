/**
 * @file ginn/xmlwishsource.cpp
 * @brief Definitions of the Ginn BAMF Wish Source class.
 */

/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ginn/xmlwishsource.h"

#include <algorithm>
#include <cstring>
#include "ginn/actionbuilder.h"
#include "ginn/configuration.h"
#include "ginn/keymap.h"
#include "ginn/wishbuilder.h"
#include "ginn/wish.h"
#include <iostream>
#include <iterator>
#include <libxml/xmlreader.h>
#include <memory>
#include <string>


/**
 * Partial specializations of std::default_delete to trick std::unique_ptr into
 * working with uninitialized pointers as singular vales.
 *
 * Sheesh, the things we do for love.
 */
namespace std
{

template<>
class default_delete<xmlRelaxNGParserCtxt>
{
public:
  void operator()(xmlRelaxNGParserCtxt* p)
  { xmlRelaxNGFreeParserCtxt(p); }
};

template<>
class default_delete<xmlRelaxNG>
{
public:
  void operator()(xmlRelaxNG* p)
  { xmlRelaxNGFree(p); }
};

template<>
class default_delete<xmlRelaxNGValidCtxt>
{
public:
  void operator()(xmlRelaxNGValidCtxt* p)
  { xmlRelaxNGFreeValidCtxt(p); }
};

template<>
class default_delete<xmlDoc>
{
public:
  void operator()(xmlDoc* p)
  { xmlFreeDoc(p); }
};

} // namespace std

namespace Ginn
{

using ParserCtxtPtr = std::unique_ptr<xmlRelaxNGParserCtxt>;
using SchemaPtr     = std::unique_ptr<xmlRelaxNG>;
using ValidatorPtr  = std::unique_ptr<xmlRelaxNGValidCtxt>;
using XmlDocPtr     = std::unique_ptr<xmlDoc>;

  
/**
 * Transforms an action XML entity into an Action.
 */
struct XmlActionBuilder
: public ActionBuilder
{
  XmlActionBuilder(xmlNodePtr const& node, Keymap* keymap);

  Action::EventList const&
  events() const;

private:
  Action::EventList events_;
};


XmlActionBuilder::
XmlActionBuilder(xmlNodePtr const& node, Keymap* keymap)
{
  Action::EventList tail;

  char const* mod1 = (char const*)xmlGetProp(node, (xmlChar const*)"modifier1");
  if (mod1)
  {
    events_.push_back({Action::EventType::key_press, keymap->to_keycode(mod1)});
    tail.push_back({Action::EventType::key_release, keymap->to_keycode(mod1)});
  }

  char const* mod2 = (char const*)xmlGetProp(node, (xmlChar const*)"modifier2");
  if (mod2)
  {
    events_.push_back({Action::EventType::key_press, keymap->to_keycode(mod2)});
    tail.push_back({Action::EventType::key_release, keymap->to_keycode(mod2)});
  }

  if (0 == strcmp((char const*)node->name, "button"))
  {
    for (xmlNodePtr child = node->children; child; child = child->next)
    {
      if (child->type == XML_TEXT_NODE)
      {
        // @todo use something better to convert content to keycode
        std::string keysym(reinterpret_cast<char const*>(child->content));
        events_.push_back({Action::EventType::button_press,
                           static_cast<Keymap::Keycode>(std::stoi(keysym))});
        tail.push_back({Action::EventType::button_release,
                        static_cast<Keymap::Keycode>(std::stoi(keysym))});
      }
    }
  }
  else if (0 == strcmp((char const*)node->name, "key"))
  {
    for (xmlNodePtr child = node->children; child; child = child->next)
    {
      if (child->type == XML_TEXT_NODE)
      {
        std::string keysym(reinterpret_cast<char const*>(child->content));
        events_.push_back({Action::EventType::key_press,
                          keymap->to_keycode(keysym)});
        tail.push_back({Action::EventType::key_release,
                        keymap->to_keycode(keysym)});
      }
    }
  }

  std::copy(tail.rbegin(), tail.rend(), std::back_inserter(events_));
}


Action::EventList const& XmlActionBuilder::
events() const
{
  return events_;
}


/**
 * Transforms a wish XML node into a Wish object.
 */
struct XmlWishBuilder
: public WishBuilder
{
  XmlWishBuilder(xmlNodePtr const& node, Keymap* keymap);

  ~XmlWishBuilder()
  { }

  std::string
  name() const
  { return gesture_ + std::to_string(touches_) + property_; }

  std::string
  gesture() const
  { return gesture_; }

  int
  touches() const
  { return touches_; }

  std::string
  when() const
  { return when_; }

  std::string
  property() const
  { return property_; }

  float
  min() const
  { return min_; }

  float
  max() const
  { return max_; }

  Action
  action() const
  { return action_; }

private:
  std::string gesture_;
  int         touches_;
  std::string when_;
  std::string property_;
  float       min_;
  float       max_;
  Action      action_;
};


/**
 * Unpacks the WIsh DOM into separate values that can be used to build a Wish.
 * @param[in] app_name The name of the application (or <global>).
 * @param[in] node     The wish XML DOM.
 */
XmlWishBuilder::
XmlWishBuilder(xmlNodePtr const& node, Keymap* keymap)
: gesture_((char const*)xmlGetProp(node, (xmlChar const*)"gesture"))
, touches_(std::stoi((char const*)xmlGetProp(node, (xmlChar const*)"fingers")))
, min_(0.0f)
, max_(0.0f)
{
  for (xmlNodePtr child = node->children; child; child = child->next)
  {
    if (child->type == XML_ELEMENT_NODE
     && 0 == strcmp((char const*)child->name, "action"))
    {
      when_ = (char const*)xmlGetProp(child, (xmlChar const*)"when");
      for (xmlNodePtr anode = child->children; anode; anode = anode->next)
      {
        if (anode->type == XML_ELEMENT_NODE)
        {
          if (0 == strcmp((char const*)anode->name, "trigger"))
          {
            property_ = (char const*)xmlGetProp(anode, (xmlChar const*)"prop");
            char const* smin = (char const*)xmlGetProp(anode, (xmlChar const*)"min");
            if (smin)
            {
              min_ = std::stof(smin);
            }
            char const* smax = (char const*)xmlGetProp(anode, (xmlChar const*)"max");
            if (smax)
            {
              max_ = std::stof(smax);
            }
          }
          else if (0 == strcmp((char const*)anode->name, "button")
                || 0 == strcmp((char const*)anode->name, "key"))
          {
            action_ = Action(XmlActionBuilder(anode, keymap));
          }
        }
      }
    }
  }
}


/**
 * Internal implementation of the XML wish source.
 */
struct XmlWishSource::Impl
{
  Impl(Configuration const& config);

  ~Impl()
  { }

  void
  wish_table_merge(Wish::Table& lhs, Wish::Table const& rhs);

  Configuration config_;
  ParserCtxtPtr ctxt_;
  SchemaPtr     schema_;
  ValidatorPtr  vctxt_;
};


/** @todo: add proper error handling to schema load/parse */
XmlWishSource::Impl::
Impl(Configuration const& config)
: config_(config)
{
  std::string const& schema_file_name = config_.wish_schema_file_name();
  if (schema_file_name != Configuration::WISH_NO_VALIDATE)
  {
    ctxt_ = ParserCtxtPtr(xmlRelaxNGNewParserCtxt(schema_file_name.c_str()));
    schema_ = SchemaPtr(xmlRelaxNGParse(ctxt_.get()));
    vctxt_ = ValidatorPtr(xmlRelaxNGNewValidCtxt(schema_.get()));
  }
}


/**
 * Merges rhs into lhs, with rhs replacing lhs where keys are dupolicated.
 * @param[inout] lhs The destination Wish::Table
 * @param[in]    rhs The source Wish::Table
 */
void XmlWishSource::Impl::
wish_table_merge(Wish::Table& lhs, Wish::Table const& rhs)
{
  for (auto const& p: rhs)
  {
    if (config_.is_verbose_mode())
    {
      std::cout << "  adding wishes for app '" << p.first << "'\n";
      for (auto const& wish: p.second)
        std::cout << "    " << *wish.second << "\n";
    }
    lhs[p.first] = p.second;
  }
}


XmlWishSource::
XmlWishSource(Configuration const& configuration)
: impl_(new Impl(configuration))
{
  if (impl_->config_.is_verbose_mode())
    std::cout << __FUNCTION__ << " created\n";
}


XmlWishSource::
~XmlWishSource()
{
}


/**
 * Processes a collection of wishes targeted to a specific application
 * (including the global <global> application).
 * @param[in]  node      An XML node to process.
 * @param[out] wishes    The current collection of processed wishes.
 */
static Wish::List
process_application_node(xmlNodePtr node, Keymap* keymap)
{
  Wish::List wish_list;
  while (node)
  {
    if (node->type == XML_ELEMENT_NODE
     && 0 == strcmp((char const*)node->name, "wish"))
    {
      auto wish = std::make_shared<Wish>(XmlWishBuilder(node, keymap));
      wish_list[wish->name()] = wish;
    }
    node = node->next;
  }
  return wish_list;
}


/**
 * Processes the top-level <ginn> node of the wish XML.
 * @param[in]  node    An XML node to process.
 * @param[in]  keymap  The mapping between key symbol names and keycodes.
 * @param[out] wishes  The current collection of processed wishes.
 */
static void
process_ginn_node(xmlNodePtr node, Keymap* keymap, Wish::Table& wish_table)
{
  while (node)
  {
    if (node->type == XML_ELEMENT_NODE)
    {
      if (0 == strcmp((char const*)node->name, "global"))
      {
        wish_table["<global>"] = process_application_node(node->children,
                                                          keymap);
      }
      else if (0 == strcmp((char const*)node->name, "applications"))
      {
        for (xmlNodePtr app_node = node->children;
             app_node;
             app_node = app_node->next)
        {
          if (app_node->type == XML_ELEMENT_NODE
           && 0 == strcmp((char const*)app_node->name, "application"))
          {
            char const* name = (char const*)xmlGetProp(app_node, (xmlChar const*)"name");
            wish_table[name] = process_application_node(app_node->children,
                                                        keymap);
          }
        }
      }
    }
    node = node->next;
  }
}


static Wish::Table
load_wishes(ValidatorPtr const&          vctxt,
            WishSource::RawSource const& raw_source,
            Keymap*                      keymap)
{
  Wish::Table wish_table;

  XmlDocPtr xml_doc { xmlParseMemory(raw_source.source.data(),
                                     raw_source.source.size()) };
  if (!xml_doc)
  {
    std::cerr << "error reading " << raw_source.name << "\n";
  }
  else
  {
    if (vctxt)
    {
      int result = xmlRelaxNGValidateDoc(vctxt.get(), xml_doc.get());
      if (result) {
        std::cerr << "validation returned " << result << "\n";
        return wish_table;
      }
    }

    xmlNodePtr root = xmlDocGetRootElement(xml_doc.get());
    if (root == NULL)
    {
      std::cerr << raw_source.name << ": empty document\n";
    }
    else if (0 != std::strcmp((char const*)root->name, "ginn"))
    {
      std::cerr << raw_source.name << ": unexpected document root '"
                << root->name << "'\n";
    }
    else
    {
      process_ginn_node(root->children, keymap, wish_table);
    }
  }

  return wish_table;
}


/**
 * Reads a wishes file and processes it into a Wish::List.
 *
 * If configured, the wish file may be validated first.
 */
Wish::Table XmlWishSource::
get_wishes(WishSource::RawSourceList const& raw_wishes, Keymap* keymap)
{
  Wish::Table wish_table;
  for (auto const& raw_source: raw_wishes)
  {
    if (impl_->config_.is_verbose_mode())
      std::cout << __FUNCTION__ << "(): "
                << " loading '" << raw_source.name << "'\n";
    impl_->wish_table_merge(wish_table,
                            load_wishes(impl_->vctxt_, raw_source, keymap));
  }
  return wish_table;
}


} // namespace Ginn

