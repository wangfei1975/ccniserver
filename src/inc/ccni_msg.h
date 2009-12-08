/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             ccni_msg.h                                                              */
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
/*             2008-11-23     initial draft                                            */
/***************************************************************************************/

#ifndef CCNI_MSG_H_
#define CCNI_MSG_H_
#include "ccni.h"
#include "xml.h"
#include "ccni_tags.h"
class CXmlMsg : public CXmlNode
{
public:
    bool addParameter(const char * paramName, int value)
    {
        char sv[16];
        sprintf(sv, "%d", value);
        return addParameter(paramName, sv);
    }
    bool addParameter(const char * paramName, const char * value)
    {
        CXmlNode dn;
        if (!dn.create(paramName))
        {
            return false;
        }

        if (!dn.setContent(value))
        {
            return false;
        }

        CXmlNode desNd = findChild(xmlTagDescription);

        if (desNd.isEmpty())
        {
            return addChild(dn);
        }

        return (xmlAddPrevSibling(desNd, dn) != NULL);
    }
};
//create ccni msg and write to net.
class CCNIMsgPacker
{
public:
    enum state_t
    {
        st_sdhd, //sending msg header. 
        st_sdbd, //reading msg body.
        st_sdok,
        st_sderror, //send socket error, socket closed or send error.
    };
private:
    CCNI_HEADER _hd;
    char * _data;
    CXmlDoc _doc;
    int _pos;
    state_t _state;
public:
    const char * data()
    {
        return _data;
    }
    CCNIMsgPacker() :
        _data(NULL), _pos(0), _state(st_sdhd)
    {
    }
    ~CCNIMsgPacker()
    {
        free();
    }
    void free()
    {
        _doc.free();
        if (_data)
        {
            xmlFree(_data);
            _data = NULL;
        }
        _pos = 0;
        _state = st_sdhd;
    }
    bool create()
    {
        CXmlNode root;
        if (!root.create(xmlTagCCNIMessages))
        {
            return false;
        }
        if (!_doc.create())
        {
            root.free();
            return false;
        }
        _doc.setRoot(root);
        return true;
    }
    bool appendmsg(CXmlNode msg)
    {
        return _doc.getRoot().addChild(msg);
    }

    bool pack(uint32_t seq, uint32_t udata, const secret_key_t & k1, const secret_key_t & k2)
    {
        int len;
        _hd.seq = seq;
        _hd.udata = udata;
        _hd.secret1 = k1;
        _hd.secret2 = k2;
        _doc.dump(_data, len);
        _hd.datalen = (uint16_t)len;
        _pos = 0;
        _state = st_sdhd;
        return (_data != NULL);
    }

    state_t send(int sock);

    int block_send(int sock);

private:
    state_t _sendsdhd(int sock);
    state_t _sendsdbd(int sock);
};

//read msg from net & parse.
class CCNIMsgParser
{
public:
    enum state_t
    {
        st_rdhd, //reading msg header, header is not ready. 
        st_rdbd, //reading msg body.
        st_rdok, //msg ok.
        st_rderror, //read socket error, socket closed or read error.
        st_hderror, //read a illegal header.
    };
private:
    CCNI_HEADER _hd;
    char * _data;
    state_t _state;
    int _pos;

    CXmlDoc _doc;
public:

    CCNIMsgParser() :
        _data(NULL), _state(st_rdhd), _pos(0)
    {
    }
    ~CCNIMsgParser()
    {
        free();
    }

    void free()
    {
        _doc.free();
        if (_data)
        {
            delete []_data;
            _data = NULL;
            _pos = 0;
            _state = st_rdhd;
        }
    }
public:
    /*
     * read data from non-blocking tcp sock fd
     *  
     * */
    bool parse()
    {
        if (_state != st_rdok)
        {
            return false;
        }
        if (!_doc.createFromXmlCdata(_data, _hd.datalen))
        {
            return false;
        }
        if (strcmp(_doc.getRoot().name(), xmlTagCCNIMessages) != 0)
        {
            LOGW("error root msg tag: %s.\n", _doc.getRoot().name());
            return false;
        }
        return true;
    }
    CXmlNode getmsg(const char * msgname)
    {
        if (_doc.isEmpty())
        {
            return NULL;
        }
        return _doc.getRoot().findChild(msgname);
    }
    CXmlNode getfirst()
    {
        return _doc.getRoot().child();
    }

    const char * data()
    {
        return _data;
    }
    state_t state()
    {
        return _state;
    }
    state_t read(int sock);
    const CCNI_HEADER & header()
    {
        return _hd;
    }
private:

    state_t _readRdhd(int sock);
    state_t _readRdbd(int sock);

};
#endif /*CCNI_MSG_H_*/
