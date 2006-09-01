/*
 * 
 * 
 * Copyright (C) 2006 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2006 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.phone.hitachi;

import org.sipfoundry.sipxconfig.phone.PhoneModel;

public final class HitachiModel extends PhoneModel {
    public static final HitachiModel MODEL_3000 = new HitachiModel("3000",
            "Hitachi Wireless IP 3000", 1);

    public static final HitachiModel MODEL_5000 = new HitachiModel("5000",
            " Hitachi Wireless IP 5000", 1);
    
    public static final HitachiModel MODEL_5000A = new HitachiModel("5000A",
    		" Hitachi Wireless IP 5000A", 1);
    
    private HitachiModel(String modelId, String label, int maxLines) {
        super(HitachiPhone.BEAN_ID, modelId, label, maxLines);
    }    
}
