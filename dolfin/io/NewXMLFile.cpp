// Copyright (C) 2009 Skavhaug.
// Licensed under the GNU LGPL Version 2.1.
//
// First added: 2009-03-03
// Last changed: 2009-03-04

#include <dolfin/common/types.h>
#include <dolfin/common/constants.h>
#include <dolfin/log/log.h>
#include "XMLArray.h"
#include "XMLMap.h"
#include "NewXMLFile.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
NewXMLFile::NewXMLFile(const std::string filename, bool gzip)
  : GenericFile(filename), sax(0)
{
  // Set up the sax handler. 
  sax = new xmlSAXHandler();
  
  // Set up handlers for parser events
  sax->startDocument = new_sax_start_document;
  sax->endDocument   = new_sax_end_document;
  sax->startElement  = new_sax_start_element;
  sax->endElement    = new_sax_end_element;
  sax->warning       = new_sax_warning;
  sax->error         = new_sax_error;
  sax->fatalError    = new_sax_fatal_error;
 
}

//-----------------------------------------------------------------------------
NewXMLFile::~NewXMLFile()
{
  delete sax; 
}
//-----------------------------------------------------------------------------
void NewXMLFile::operator>>(std::vector<int>& x)
{
  message(1, "Reading array from file %s.", filename.c_str());
  XMLArray xml_array(x, *this);
  xml_array.handle();
  parse();
  if ( !handlers.empty() ) 
    error("Hander stack not empty. Something is wrong!");
}
//-----------------------------------------------------------------------------
void NewXMLFile::operator>>(std::vector<uint>& x)
{
  message(1, "Reading array from file %s.", filename.c_str());
  XMLArray xml_array(x, *this);
  xml_array.handle();
  parse();
  if ( !handlers.empty() ) 
    error("Hander stack not empty. Something is wrong!");
}
//-----------------------------------------------------------------------------
void NewXMLFile::operator>>(std::vector<double>& x)
{
  message(1, "Reading array from file %s.", filename.c_str());
  XMLArray xml_array(x, *this);
  xml_array.handle();
  parse();
  if ( !handlers.empty() ) 
    error("Hander stack not empty. Something is wrong!");
}
//-----------------------------------------------------------------------------
void NewXMLFile::operator>>(std::map<uint, std::vector<int> >& array_map)
{
  message(1, "Reading array map from file %s.", filename.c_str());
  XMLMap xml_array_map(array_map, *this);
  xml_array_map.handle();
  parse();
  if ( !handlers.empty() ) 
    error("Hander stack not empty. Something is wrong!");
}
//-----------------------------------------------------------------------------
void NewXMLFile::operator>>(std::map<uint, std::vector<uint> >& array_map)
{
  message(1, "Reading array map from file %s.", filename.c_str());
  XMLMap xml_array_map(array_map, *this);
  xml_array_map.handle();
  parse();
  if ( !handlers.empty() ) 
    error("Hander stack not empty. Something is wrong!");
}
//-----------------------------------------------------------------------------
void NewXMLFile::operator>>(std::map<uint, std::vector<double> >& array_map)
{
  message(1, "Reading array map from file %s.", filename.c_str());
  XMLMap xml_array_map(array_map, *this);
  xml_array_map.handle();
  parse();
  if ( !handlers.empty() ) 
    error("Hander stack not empty. Something is wrong!");
}
//-----------------------------------------------------------------------------
void NewXMLFile::operator<<(const std::vector<int>& x)
{
  open_file();
  XMLArray::write(x, outfile);
  close_file();
}
//-----------------------------------------------------------------------------
void NewXMLFile::operator<<(const std::vector<uint>& x)
{
  open_file();
  XMLArray::write(x, outfile);
  close_file();
}
//-----------------------------------------------------------------------------
void NewXMLFile::operator<<(const std::vector<double>& x)
{
  open_file();
  XMLArray::write(x, outfile);
  close_file();
}
//-----------------------------------------------------------------------------
void NewXMLFile::operator<<(const std::map<uint, std::vector<int> >& array_map)
{
}
//-----------------------------------------------------------------------------
void NewXMLFile::operator<<(const std::map<uint, std::vector<uint> >& array_map)
{
}
//-----------------------------------------------------------------------------
void NewXMLFile::operator<<(const std::map<uint, std::vector<double> >& array_map)
{
}
//-----------------------------------------------------------------------------
void NewXMLFile::parse()
{
  // Parse file
  xmlSAXUserParseFile(sax, (void *) this, filename.c_str());

}
//-----------------------------------------------------------------------------
void NewXMLFile::push(XMLHandler* handler)
{
  handlers.push(handler);
}
//-----------------------------------------------------------------------------
void NewXMLFile::pop()
{
  dolfin_assert( !handlers.empty() );
  handlers.pop();
}
//-----------------------------------------------------------------------------
XMLHandler* NewXMLFile:: top()
{
  dolfin_assert( !handlers.empty() );
  return handlers.top();
}
//-----------------------------------------------------------------------------
void NewXMLFile::start_element(const xmlChar *name, const xmlChar **attrs)
{
  handlers.top()->start_element(name, attrs);
}
//-----------------------------------------------------------------------------
void NewXMLFile::end_element(const xmlChar *name)
{
  handlers.top()->end_element(name);
}
//-----------------------------------------------------------------------------
void NewXMLFile::open_file()
{
  // Open file
  outfile.open(filename.c_str());

  // Go to end of file
  outfile.seekp(0, std::ios::end);
}

//-----------------------------------------------------------------------------
// Callback functions for the SAX interface
//-----------------------------------------------------------------------------
void dolfin::new_sax_start_document(void *ctx)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void dolfin::new_sax_end_document(void *ctx)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void dolfin::new_sax_start_element(void *ctx,
			       const xmlChar *name, const xmlChar **attrs)
{
  ( (NewXMLFile*) ctx )->start_element(name, attrs);
}
//-----------------------------------------------------------------------------
void dolfin::new_sax_end_element(void *ctx, const xmlChar *name)
{
  ( (NewXMLFile*) ctx )->end_element(name);
}
//-----------------------------------------------------------------------------
void dolfin::new_sax_warning(void *ctx, const char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  char buffer[DOLFIN_LINELENGTH];
  vsnprintf(buffer, DOLFIN_LINELENGTH, msg, args);
  warning("Incomplete XML data: " + std::string(buffer));
  va_end(args);
}
//-----------------------------------------------------------------------------
void dolfin::new_sax_error(void *ctx, const char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  char buffer[DOLFIN_LINELENGTH];
  vsnprintf(buffer, DOLFIN_LINELENGTH, msg, args);
  error("Illegal XML data: " + std::string(buffer));
  va_end(args);
}
//-----------------------------------------------------------------------------
void dolfin::new_sax_fatal_error(void *ctx, const char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  char buffer[DOLFIN_LINELENGTH];
  vsnprintf(buffer, DOLFIN_LINELENGTH, msg, args);
  error("Illegal XML data: " + std::string(buffer));
  va_end(args);
}
//-----------------------------------------------------------------------------
