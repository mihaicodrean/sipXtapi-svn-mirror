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
package org.sipfoundry.sipxconfig.common.event;

import java.util.Collection;
import java.util.Iterator;
import java.util.Map;

import org.springframework.beans.factory.BeanInitializationException;
import org.springframework.beans.factory.ListableBeanFactory;
import org.springframework.context.ApplicationEvent;
import org.springframework.context.ApplicationListener;
import org.springframework.context.event.ContextRefreshedEvent;

/**
 * Publisher for Dao events
 * 
 * Implementation: we could probably use Spring application context event publishing facility but
 * it would require that our event handling is aware of ApplicationEvent class.
 */
public class DaoEventPublisherImpl implements DaoEventPublisher, ApplicationListener {
    private Collection m_listeners;

    private ListableBeanFactory m_beanFactory;

    /**
     * Lazily creates the collection of beans that implement DaoEventListener interface
     * 
     * @return cached or newly created listener collection
     */
    private Collection getListeners() {
        if (m_listeners == null) {
            if (m_beanFactory == null) {
                throw new BeanInitializationException(getClass().getName() + " not initialized");
            }
            Map beanMap = m_beanFactory.getBeansOfType(DaoEventListener.class, true, true);
            m_listeners = beanMap.values();
        }
        return m_listeners;
    }

    public void publishDelete(Object entity) {
        for (Iterator i = getListeners().iterator(); i.hasNext();) {
            DaoEventListener listener = (DaoEventListener) i.next();
            listener.onDelete(entity);
        }
    }

    public void publishSave(Object entity) {
        for (Iterator i = getListeners().iterator(); i.hasNext();) {
            DaoEventListener listener = (DaoEventListener) i.next();
            listener.onSave(entity);
        }
    }

    /**
     * Updates reference to bean factory and cleans the cache of listeners
     */
    public void onApplicationEvent(ApplicationEvent event) {
        if (event instanceof ContextRefreshedEvent) {
            ContextRefreshedEvent cre = (ContextRefreshedEvent) event;
            m_beanFactory = cre.getApplicationContext();
            m_listeners = null;
        }
    }
}
