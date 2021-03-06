/*
  Copyright (C) 2009  Wang Fei (bjwf2000@gmail.com)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Generl Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             xml.cpp                                                                 */
/*                                                                                     */
/*  version                                                                            */
/*             1.0                                                                     */
/*                                                                                     */
/*  description                                                                        */
/*                                                                                     */
/*                                                                                     */
/*  component                                                                          */
/*                                                                                     */
/*  author                                                                             */
/*             bjwf       bjwf2000@gmail.com                                           */
/*                                                                                     */
/*  histroy                                                                            */
/*             2008-11-22     initial draft                                            */
/***************************************************************************************/
#include "xml.h"
#include "utils.h"
///////////////////////////////////////////////////////////////////////////////
//
bool CXmlDoc::create()
{
    _doc = xmlNewDoc(BAD_CAST("1.0"));
    if (_doc == NULL)
    {
        return false;
    }
    //_doc->encoding = "gb2312";
    // xmlNodePtr nd = xmlNewNode(NULL, BAD_CAST(xmlTagCCNIMessages));
    // if (nd == NULL)
    {
        //    xmlFreeDoc(_doc);
        //     return false;
    }

    // xmlDocSetRootElement(_doc, nd);
    return true;

}
bool CXmlDoc::createFromXmlCdata(const char * xmlstring, int len)
{
    //   xmlCleanupParser();
    _doc = xmlReadMemory(xmlstring, len, "noname.xml", NULL, 0);
    if (_doc == NULL)
    {
        return false;
    }
    return true;
}
bool CXmlDoc::createFromFile(const char * fname)
{
    _doc = xmlParseFile(fname);
    if (_doc == NULL)
    {
        return false;
    }
    return true;
}
CXmlNode CXmlDoc::getRoot()
{ 
    if (isEmpty())
    {
        return NULL;
    }
    return xmlDocGetRootElement(_doc);

}
void CXmlDoc::free()
{
    if (_doc)
    {
        xmlFreeDoc(_doc);
        _doc = NULL;
    }
}

std::string & CXmlDoc::toString(std::string & strXml)
{
    xmlChar *xmlbuff;
    int buffersize;
    xmlDocDumpFormatMemoryEnc(_doc, &xmlbuff, &buffersize, "UTF-8", 1);
    strXml = (const char *)xmlbuff;
    xmlFree(xmlbuff);
    return strXml;
}
///////////////////////////////////////////////////////////////////////////////
//

bool CXmlNode::create(const char * name)
{
    _node = xmlNewNode(NULL, BAD_CAST(name));
    return !isEmpty();
}

bool CXmlNode::create()
{
    return create("");
}
 
void CXmlNode::free()
{
    xmlFreeNode(_node);
    _node = NULL;
}
CXmlNode CXmlNode::attach(xmlNodePtr nd)
{
    _node = nd;
    return *this;
}

void CXmlNode::detach()
{
    _node = NULL;
}
int CXmlNode::getIntContent()
{
    int v = 0;
    xmlChar * p = xmlNodeGetContent(_node);
    if (p != NULL)
    {
        sscanf((char *)p, "%d", &v);
    }
    xmlFree(p);
    return v;
}

std::string & CXmlNode::getContent(std::string & v)
{
    v.clear();
    xmlChar * p = xmlNodeGetContent(_node);
    if (p != NULL)
    {
        v = (char *)p;
        xmlFree(p);
    }
    return v;

}
bool CXmlNode::setContent(const char * content)
{
    if (isEmpty())
    {
        return false;
    }
    xmlNodeSetContent(_node, BAD_CAST(content));
    //std::string buf;

    //xmlNodeSetContent(_node, BAD_CAST(gb23122utf8(buf, content).c_str()));
    return true;
}

bool CXmlNode::setName(const char * name)
{
    if (isEmpty())
    {
        return false;
    }

    xmlNodeSetName(_node, BAD_CAST(name));
    return true;
}

bool CXmlNode::addProp(const char * propName, const char * value)
{
    if (isEmpty())
    {
        return false;
    }
    xmlNewProp(_node, BAD_CAST(propName), BAD_CAST(value));
    return true;
}
std::string & CXmlNode::getProp(const char * propName, std::string &v)
{
    v.clear();
    if (isEmpty())
    {
        return v;
    }
    xmlChar * p = xmlGetProp(_node, BAD_CAST(propName));
    if (p)
    {
        v = (char *)p;
        xmlFree(p);
    }
    return v;

}

bool CXmlNode::addChild(CXmlNode child)
{
    if (isEmpty())
    {
        return false;
    }

    xmlAddChild(_node, child);
    return true;

}

CXmlNode CXmlNode::findChild(const char * name) const
{
    if (isEmpty())
    {
        return NULL;
    }

    xmlNodePtr child = _node->children;
    while (child != NULL)
    {
        if (child->type == XML_ELEMENT_NODE)
        {
            if (strcmp((const char *)child->name, name) == 0)
            {
                return (child);
            }
        }
        child = child->next;
    }
    return NULL;
}

std::string & CXmlNode::toString(std::string & strXml)
{
    xmlBufferPtr buf = xmlBufferCreate();
    xmlNodeDump(buf, NULL, _node, 0, 1);
    utf82gb2312(strXml, (const char *)xmlBufferContent(buf));
    xmlBufferFree(buf);
    return strXml;
}

