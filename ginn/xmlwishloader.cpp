/**
 * @file ginn/xmlwishloader.cpp
 * @brief Definitions of the Ginn BAMF Wish Loader class.
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
#include "ginn/xmlwishloader.h"

#include <cstring>
#include "ginn/wishbuilder.h"
#include "ginn/wish.h"
#include <iostream>
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

typedef std::unique_ptr<xmlRelaxNGParserCtxt> ParserCtxtPtr;
typedef std::unique_ptr<xmlRelaxNG>           SchemaPtr;
typedef std::unique_ptr<xmlRelaxNGValidCtxt>  ValidatorPtr;
typedef std::unique_ptr<xmlDoc>               XmlDocPtr;

  
/**
 * Transforms a wish XML node into a Wish object.
 */
struct XmlWishBuilder
: public WishBuilder
{
  XmlWishBuilder(xmlNodePtr const& node);

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
  { return 0.0f; }

  float
  max() const
  { return 0.0f; }

  std::string
  action() const
  { return action_; }

  std::string
  modifier1() const
  { return ""; }

  std::string
  modifier2() const
  { return ""; }

  std::string
  modifier3() const
  { return ""; }

  std::string gesture_;
  int         touches_;
  std::string when_;
  std::string property_;
  std::string action_;
};


/**
 * Unpacks the WIsh DOM into separate values that can be used to build a Wish.
 * @param[in] app_name The name of the application (or <global>).
 * @param[in] node     The wish XML DOM.
 */
XmlWishBuilder::
XmlWishBuilder(xmlNodePtr const& node)
: gesture_((char const*)xmlGetProp(node, (xmlChar const*)"gesture"))
, touches_(std::stoi((char const*)xmlGetProp(node, (xmlChar const*)"fingers")))
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
          }
          else if (0 == strcmp((char const*)anode->name, "button"))
          {
            action_ = "button";
          }
          else if (0 == strcmp((char const*)anode->name, "key"))
          {
            action_ = "key";
          }
        }
      }
    }
  }
}


/**
 * Internal implementation of the XML wish loader.
 */
struct XmlWishLoader::Impl
{
  /** @todo: add proper error handling to schema load/parse */
  Impl(std::string const& schema_file_name)
  {
    if (schema_file_name != Configuration::WISH_NO_VALIDATE)
    {
      ctxt_ = ParserCtxtPtr(xmlRelaxNGNewParserCtxt(schema_file_name.c_str()));
      schema_ = SchemaPtr(xmlRelaxNGParse(ctxt_.get()));
      vctxt_ = ValidatorPtr(xmlRelaxNGNewValidCtxt(schema_.get()));
    }
  }

  ~Impl()
  { }

  ParserCtxtPtr ctxt_;
  SchemaPtr     schema_;
  ValidatorPtr  vctxt_;
};


XmlWishLoader::
XmlWishLoader(std::string const& wish_schema_file_name)
: impl_(new Impl(wish_schema_file_name))
{
}


XmlWishLoader::
~XmlWishLoader()
{
}


/**
 * Processes a collection of wishes targeted to a specific application
 * (including the global <global> application).
 * @param[in]  node      An XML node to process.
 * @param[out] wishes    The current collection of processed wishes.
 */
static Wish::List
process_application_node(xmlNodePtr node)
{
  Wish::List wish_list;
  while (node)
  {
    if (node->type == XML_ELEMENT_NODE
     && 0 == strcmp((char const*)node->name, "wish"))
    {
      auto wish = std::make_shared<Wish>(XmlWishBuilder(node));
      wish_list[wish->name()] = wish;
    }
    node = node->next;
  }
  return wish_list;
}


/**
 * Processes the top-level <ginn> node of the wish XML.
 * @param[in]  node   An XML node to process.
 * @param[out] wishes The current collection of processed wishes.
 */
static void
process_ginn_node(xmlNodePtr node, Wish::Table& wish_table)
{
  while (node)
  {
    if (node->type == XML_ELEMENT_NODE)
    {
      if (0 == strcmp((char const*)node->name, "global"))
      {
        wish_table["<global>"] = process_application_node(node->children);
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
            wish_table[name] = process_application_node(app_node->children);
          }
        }
      }
    }
    node = node->next;
  }
}


/**
 * Merges rhs into lhs, with rhs replacing lhs where keys are dupolicated.
 * @param[inout] lhs The destination Wish::Table
 * @param[in]    rhs The source Wish::Table
 */
static void
wish_table_merge(Wish::Table& lhs, Wish::Table const& rhs)
{
  for (auto const& p: rhs)
    lhs[p.first] = p.second;
}


static Wish::Table
load_wishes(ValidatorPtr const& vctxt, std::string const& wish_file_name)
{
  Wish::Table wish_table;

  XmlDocPtr xml_doc { xmlParseFile(wish_file_name.c_str()) };
  if (!xml_doc)
  {
    std::cerr << "error reading " << wish_file_name << "\n";
  }
  else
  {
    if (vctxt)
    {
      int result = xmlRelaxNGValidateDoc(vctxt.get(), xml_doc.get());
      if (result) {
        std::cerr << "validation returned " << result << "\n";
      }
      else
      {
        xmlNodePtr root = xmlDocGetRootElement(xml_doc.get());
        if (root == NULL)
        {
          std::cerr << wish_file_name << ": empty document\n";
        }
        else if (0 != std::strcmp((char const*)root->name, "ginn"))
        {
          std::cerr << wish_file_name << ": unexpected document root '"
                    << root->name << "'\n";
        }
        else
        {
          process_ginn_node(root->children, wish_table);
        }
      }
    }
  }

  return wish_table;
}


/**
 * Reads a wishes file and processes it into a Wish::List.
 *
 * If configured, the wish file may be validated first.
 */
Wish::Table XmlWishLoader::
get_wishes(FileNameList const& wish_file_names)
{
  Wish::Table wish_table;
  for (auto const& wish_file_name: wish_file_names)
  {
    wish_table_merge(wish_table, load_wishes(impl_->vctxt_, wish_file_name));
  }
  return wish_table;
}


} // namespace Ginn

