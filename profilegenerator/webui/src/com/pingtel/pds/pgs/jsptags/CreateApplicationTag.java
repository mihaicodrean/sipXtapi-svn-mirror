/*
 * $Id: //depot/OPENDEV/sipXconfig/profilegenerator/webui/src/com/pingtel/pds/pgs/jsptags/CreateApplicationTag.java#4 $
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $$
 */

package com.pingtel.pds.pgs.jsptags;

import com.pingtel.pds.common.EJBHomeFactory;
import com.pingtel.pds.common.RedirectServletException;
import com.pingtel.pds.pgs.jsptags.util.ExTagSupport;
import com.pingtel.pds.pgs.user.ApplicationAdvocate;
import com.pingtel.pds.pgs.user.ApplicationAdvocateHome;
import com.pingtel.pds.pgs.beans.CreateApplicationBean;

import javax.servlet.jsp.JspException;
import javax.servlet.jsp.PageContext;


public class CreateApplicationTag extends ExTagSupport {

    private String m_orgID;
    private String m_name;
    private String m_deviceTypeID;
    private String m_refPropertyID;
    private String m_url;
    private String m_description;

    private ApplicationAdvocateHome aaHome = null;
    private ApplicationAdvocate aaAdvocate = null;


    public void setOrgid( String orgid ){
        m_orgID = orgid;
    }

    public void setName ( String name ) {
        m_name = name;
    }

    public void setDevicetypeid ( String devicetypeid) {
        this.m_deviceTypeID = devicetypeid;
    }

    public void setRefpropertyid ( String refpropertyid) {
        this.m_refPropertyID = refpropertyid;
    }

    public void setUrl ( String url) {
        this.m_url = url;
    }

    public void setDescription ( String description) {
        this.m_description = description;
    }


    public int doStartTag() throws JspException {
        try {
            if ( aaHome == null ) {
                aaHome = ( ApplicationAdvocateHome )
                    EJBHomeFactory.getInstance().getHomeInterface(  ApplicationAdvocateHome.class,
                                                                    "ApplicationAdvocate" );

                aaAdvocate = aaHome.create();
            }

            aaAdvocate.createApplication(   m_name,
                                            this.getOrganizationID(),
                                            m_deviceTypeID,
                                            m_refPropertyID,
                                            m_url,
                                            m_description );

        }
        catch (Exception ex ) {
            CreateApplicationBean createApplicationBean = new CreateApplicationBean();
            createApplicationBean.setName(m_name);
            createApplicationBean.setUrl((m_url));
            createApplicationBean.setDescription(m_description);
            createApplicationBean.setErrorMessage(ex.getMessage());

            pageContext.setAttribute("CreateApplicationBean", createApplicationBean, PageContext.REQUEST_SCOPE);

            throw new RedirectServletException(ex.getMessage(), "../popup/form_add_application.jsp", null);
        }

        return SKIP_BODY;
    }

    protected void clearProperties()
    {
        m_orgID = null;
        m_name  = null;
        m_deviceTypeID = null;
        m_refPropertyID = null;
        m_url = null;
        m_description = null;

        super.clearProperties();
    }
}
