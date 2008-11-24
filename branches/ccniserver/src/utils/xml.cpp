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
    CXmlNode nd;
    if (isEmpty())
    {
        return nd;
    }
    nd.attach(xmlDocGetRootElement(_doc));
    return nd;

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
int  CXmlNode::getIntContent()
{
    int v = 0;
    sscanf(content(), "%d", &v);
    return v;
}
 
std::string & CXmlNode::getContent(std::string & v)
{
    v.clear();
    if (isEmpty())
    {
        return v;
    }
    v = content();
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

