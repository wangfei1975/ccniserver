/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             ccni_xml.h                                                              */
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
/*             2008-11-21     initial draft                                            */
/***************************************************************************************/

#include "ccni_xml.h"

static std::string & gb23122utf8(std::string & dst, const char * src);
static std::string & utf82gb2312(std::string & dst, const char * src);

///////////////////////////////////////////////////////////////////////////////
//
CCNIXmlDoc::CCNIXmlDoc() :
    _doc(NULL)
{

}

bool CCNIXmlDoc::create()
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
bool CCNIXmlDoc::createFromXmlCdata(const char * xmlstring, int len)
{
    //   xmlCleanupParser();
    _doc = xmlReadMemory(xmlstring, len, "noname.xml", NULL, 0);
    if (_doc == NULL)
    {
        return false;
    }
    //  xmlNodePtr root = xmlDocGetRootElement(_doc);
    //    if (root == NULL || strcmp((const char *)root->name, xmlTagCCNIMessages) != 0)
    //    {
    //        free();
    //        return false;
    //    }
    return true;
}

void CCNIXmlDoc::free()
{
    if (_doc)
    {
        xmlFreeDoc(_doc);
        _doc = NULL;
    }
}
void CCNIXmlDoc::attach(xmlDocPtr doc)
{
    _doc = doc;
}
void CCNIXmlDoc::detach()
{
    _doc = NULL;
}

bool CCNIXmlDoc::appendMsg(CCNIXmlMsg msg)
{
    if (isEmpty())
    {
        return false;
    }

    xmlNode * root = xmlDocGetRootElement(_doc);
    if (root == NULL)
    {
        return false;
    }

    if (xmlAddChild(root, msg) == NULL)
    {
        return false;
    }
    msg.detach();
    return true;

}

int CCNIXmlDoc::getMsgCount()
{
    xmlNodePtr root = xmlDocGetRootElement(_doc);
    if (root == NULL)
    {
        return 0;
    }

    int cnt = 0;
    for (xmlNodePtr child = root->children; child != NULL && child->type == XML_ELEMENT_NODE; child
            = child->next)
    {
        cnt++;
    }

    return cnt;
}
CCNIXmlMsg CCNIXmlDoc::getRoot()
{
    CCNIXmlMsg msg;
    xmlNodePtr root = xmlDocGetRootElement(_doc);
    if (root)
    {
        msg.attach(root);
    }
    return msg;

}
CCNIXmlMsg CCNIXmlDoc::getFirstMsg()
{
    CCNIXmlMsg msg;
    xmlNodePtr root = xmlDocGetRootElement(_doc);
    if (root == NULL || strcmp((const char *)root->name, "CCNIMessages") != 0)
    {
        return msg;
    }
    xmlNodePtr child = root->children;
    while (child != NULL && child->type != XML_ELEMENT_NODE)
    {
        child = child->next;
    }
    msg.attach(child);
    return msg;
}

CCNIXmlMsg CCNIXmlDoc::getNextMsg(CCNIXmlMsg cur)
{
    CCNIXmlMsg msg;
    if (isEmpty())
    {
        return msg;
    }
    if (cur == NULL)
    {
        return msg;
    }
    xmlNodePtr child = (xmlNodePtr(cur))->next;
    while (child != NULL && child->type != XML_ELEMENT_NODE)
    {
        child = child->next;
    }
    msg.attach(child);
    return msg;
}
std::string & CCNIXmlDoc::toUtf8String(std::string & strXml)
{
    xmlChar *xmlbuff;
    int buffersize;
    xmlDocDumpFormatMemoryEnc(_doc, &xmlbuff, &buffersize, "UTF-8", 1);
    strXml = (const char *)xmlbuff;
    xmlFree(xmlbuff);
    return strXml;

}
std::string & CCNIXmlDoc::toString(std::string & strXml)
{
    xmlChar *xmlbuff;
    int buffersize;
    xmlDocDumpFormatMemoryEnc(_doc, &xmlbuff, &buffersize, "gb2312", 1);
    strXml = (const char *)xmlbuff;
    xmlFree(xmlbuff);
    return strXml;
    /*
     std::string tmp;
     toUtf8String(tmp);
     return utf82gb2312(strXml, (const char *)tmp.c_str());
     */
}
///////////////////////////////////////////////////////////////////////////////
//
CCNIXmlNode::CCNIXmlNode() :
    _node(NULL)
{
}
CCNIXmlNode::CCNIXmlNode(xmlNodePtr nd) :
    _node(nd)
{
}
CCNIXmlNode::~CCNIXmlNode()
{
}
bool CCNIXmlNode::create(const char * name)
{
    _node = xmlNewNode(NULL, BAD_CAST(name));
    return !isEmpty();
}

bool CCNIXmlNode::create()
{
    return create("");
}
void CCNIXmlNode::free()
{
    xmlFreeNode(_node);
    _node = NULL;
}

void CCNIXmlNode::attach(xmlNodePtr nd)
{
    _node = nd;
}

void CCNIXmlNode::detach()
{
    _node = NULL;
}

std::string & CCNIXmlNode::getContent(std::string & v)
{
    if (isEmpty())
    {
        return v;
    }
    return utf82gb2312(v, (const char *)xmlNodeGetContent(_node));

}
bool CCNIXmlNode::setContent(const char * content)
{
    if (isEmpty())
    {
        return false;
    }
    std::string buf;

    xmlNodeSetContent(_node, BAD_CAST(gb23122utf8(buf, content).c_str()));
    return true;
}

bool CCNIXmlNode::setName(const char * name)
{
    if (isEmpty())
    {
        return false;
    }

    xmlNodeSetName(_node, BAD_CAST(name));
    return true;
}

bool CCNIXmlNode::addProp(const char * propName, const char * value)
{
    if (isEmpty())
    {
        return false;
    }
    xmlNewProp(_node, BAD_CAST propName, BAD_CAST value);
    return true;
}
const char * CCNIXmlNode::getProp(const char * propName)
{
    return NULL;
}
///////////////////////////////////////////////////////////////////////////////
//

bool CCNIXmlNodeTree::addChild(CCNIXmlNode child)
{
    if (isEmpty())
    {
        return false;
    }

    xmlAddChild(_node, child);
    return true;

}
bool CCNIXmlNodeTree::addChild(CCNIXmlNodeTree child)
{

    return addChild((CCNIXmlNode)(child));
}

CCNIXmlNodeTree CCNIXmlNodeTree::findChildElement(const char * name) const
{
    CCNIXmlNodeTree nd;
    if (isEmpty())
    {
        return nd;
    }

    xmlNodePtr child = _node->children;
    while (child != NULL)
    {
        if (child->type == XML_ELEMENT_NODE)
        {
            if (strcmp((const char *)child->name, name) == 0)
            {
                nd.attach(child);
                return nd;
            }
        }
        child = child->next;
    }
    return nd;
}

std::string & CCNIXmlNodeTree::toString(std::string & strXml)
{
    xmlBufferPtr buf = xmlBufferCreate();
    xmlNodeDump(buf, NULL, _node, 0, 1);
    utf82gb2312(strXml, (const char *)xmlBufferContent(buf));
    xmlBufferFree(buf);
    return strXml;
}

///////////////////////////////////////////////////////////////////////////////
//

bool CCNIXmlMsg::setLabel(const char * ccniMsgLabel)
{
    return setName(ccniMsgLabel);
}

bool CCNIXmlMsg::setDescription(const char * des)
{

    CCNIXmlNode dn = findChildElement("Description");
    if (dn.isEmpty())
    {
        if (!dn.create("Description"))
        {
            return false;
        }
        if (!dn.setContent(des))
        {
            return false;
        }

        return addChild(dn);
    }
    else
    {
        return dn.setContent(des);
    }
}

bool CCNIXmlMsg::addParameter(const char * paramName, const char * value)
{
    CCNIXmlNode dn;
    if (!dn.create(paramName))
        return false;

    if (!dn.setContent(value))
        return false;

    CCNIXmlNode desNd = findChildElement("Description");

    if (desNd.isEmpty())
    {
        return addChild(dn);
    }

    return (xmlAddPrevSibling(desNd, dn) != NULL);
}

bool CCNIXmlMsg::addParameter(CCNIXmlNode param)
{

    CCNIXmlNode desNd = findChildElement("Description");

    if (desNd.isEmpty())
    {
        return addChild(param);
    }

    return (xmlAddPrevSibling(desNd, param) != NULL);
}
bool CCNIXmlMsg::setParameter(const char * paramName, const char * value)
{
    CCNIXmlNode nd = findChildElement(paramName);
    if (nd.isEmpty())
    {
        return addParameter(paramName, value);
    }

    return nd.setContent(value);
}
bool CCNIXmlMsg::setParameter(const char * paramName, int v)
{
    char sv[20];
    sprintf(sv, "%d", v);
    return setParameter(paramName, sv);
}
const char * CCNIXmlMsg::getLabel() const
{
    if (_node)
    {
        return (const char *)_node->name;
    }
    return NULL;
}

std::string & CCNIXmlMsg::getTextParameter(const char * paramName, std::string & value) const
{
    CCNIXmlNodeTree nd = findChildElement(paramName);
    value.erase(value.begin(), value.end());
    if (nd.isEmpty())
    {
        return value;
    }
    return nd.getContent(value);
}

int CCNIXmlMsg::getIntParameter(const char * paramName) const
{
    int i = 0;
    CCNIXmlNodeTree nd = findChildElement(paramName);
    if (nd.isEmpty())
    {
        return 0;
    }
    std::string v;
    nd.getContent(v);

    sscanf(v.c_str(), "%d", &i);
    return i;
}
bool CCNIXmlMsg::getParameter(const char * paramName, std::string & v) const
{
    CCNIXmlNodeTree nd = findChildElement(paramName);
    if (nd.isEmpty())
    {
        return false;
    }

    nd.getContent(v);

    return true;
}
bool CCNIXmlMsg::getParameter(const char * paramName, int & v) const
{
    CCNIXmlNodeTree nd = findChildElement(paramName);
    if (nd.isEmpty())
    {
        return false;
    }

    std::string sv;
    nd.getContent(sv);

    if (sscanf(sv.c_str(), "%d", &v) != 1)
    {
        return false;
    }

    return true;
}
CCNIXmlMsg CCNIXmlMsg::getComplexParameter(const char * paramName) const
{
    CCNIXmlMsg m;
    m.attach(findChildElement(paramName));
    return m;
}
///////////////////////////////////////////////////////////////////////////////
//
//
static std::string & gb23122utf8(std::string & dst, const char * src)
{
    iconv_t icpt;

    size_t inlen = strlen(src), outlen;
    dst.erase(dst.begin(), dst.end());

    if ((icpt = iconv_open("utf-8", "gb2312")) == (iconv_t)-1)
    {
        return dst;
    }
    outlen = inlen*2;
    char * out = new char[outlen+1];
    char * sout = out;
    int ret;
    if ((ret = (int)iconv(icpt, (char **)&src, &inlen, &sout, &outlen)) != -1)
    {
        *sout = 0;
        dst = out;
    }

    delete []out;
    iconv_close(icpt);
    return dst;
}
static std::string & utf82gb2312(std::string & dst, const char * src)
{
    iconv_t icpt;

    size_t inlen = strlen(src), outlen;
    dst.erase(dst.begin(), dst.end());

    if ((icpt = iconv_open("gb2312", "utf-8")) == (iconv_t)-1)
    {
        return dst;
    }
    outlen = inlen;
    char * out = new char[outlen+1];
    char * sout = out;
    int ret;
    if ((ret = (int)iconv(icpt, (char **)&src, &inlen, &sout, &outlen)) != -1)
    {
        *sout = 0;
        dst = out;
    }

    delete []out;
    iconv_close(icpt);
    return dst;

}

