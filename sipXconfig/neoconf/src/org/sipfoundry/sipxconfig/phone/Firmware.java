/*
 * 
 * 
 * Copyright (C) 2005 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2005 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.phone;

import org.sipfoundry.sipxconfig.setting.BeanWithSettings;
import org.sipfoundry.sipxconfig.setting.ModelFilesContext;
import org.sipfoundry.sipxconfig.setting.Setting;

/**
 * Describing the files required to track and manage a vendor's firmware files
 */
public class Firmware extends BeanWithSettings {
    private String m_name;
    private String m_description;
    private String m_deliveryId;
    private String m_manufacturerId;
    private String m_modelId;
    private String m_versionId;
    private ModelFilesContext m_modelFilesContext;
    
    public Setting getSettingModel() {
        Setting model = super.getSettingModel();
        if (model == null) {
            // TODO: Details
            model = m_modelFilesContext.loadModelFile("firmware.xml", getManufacturerId(), null);
            setSettingModel(model);
        }
        return model;
    }
    
    public String getName() {
        return m_name;
    }

    public void setName(String name) {
        m_name = name;
    }

    public String getDescription() {
        return m_description;
    }
    public void setDescription(String description) {
        m_description = description;
    }
    public String getManufacturerId() {
        return m_manufacturerId;
    }
    public void setManufacturerId(String manufacturerId) {
        m_manufacturerId = manufacturerId;
    }
    public String getModelId() {
        return m_modelId;
    }
    public void setModelId(String modelId) {
        m_modelId = modelId;
    }
    public String getVersionId() {
        return m_versionId;
    }
    public void setVersionId(String versionId) {
        m_versionId = versionId;
    }
    public String getDeliveryId() {
        return m_deliveryId;
    }
    public void setDeliveryId(String deliveryId) {
        m_deliveryId = deliveryId;
    }
    public ModelFilesContext getModelFilesContext() {
        return m_modelFilesContext;
    }
    public void setModelFilesContext(ModelFilesContext modelFilesContext) {
        m_modelFilesContext = modelFilesContext;
    }
}
