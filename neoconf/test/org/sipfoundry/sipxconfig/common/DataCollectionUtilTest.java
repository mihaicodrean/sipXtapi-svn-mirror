/*
 * 
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.common;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

import junit.framework.TestCase;

import org.apache.commons.lang.ArrayUtils;

public class DataCollectionUtilTest extends TestCase {

    private List m_items;

    private Integer[] m_primaryKeys;

    private int m_originalSize;

    protected void setUp() {
        m_items = new ArrayList();
        for (int i = 0; i < 40; i++) {
            m_items.add(new TestCollectionItem(i, i));
        }

        // out of order on purpose
        m_primaryKeys = new Integer[] {
            new Integer(30), new Integer(10), new Integer(20)
        };

        m_originalSize = m_items.size();
    }

    public void testFindByPrimaryKey() {
        Collection found = DataCollectionUtil.findByPrimaryKey(m_items, m_primaryKeys);
        assertEquals(m_primaryKeys.length, found.size());
        Iterator ifound = found.iterator();
        Arrays.sort(m_primaryKeys);
        for (int i = 0; ifound.hasNext(); i++) {
            DataCollectionItem item = (DataCollectionItem) ifound.next();
            assertTrue(Arrays.binarySearch(m_primaryKeys, item.getPrimaryKey()) >= 0);
        }
    }

    public void testRemoveByPrimaryKey() {
        // assumes findByPrimaryKey works
        Iterator find = DataCollectionUtil.findByPrimaryKey(m_items, m_primaryKeys).iterator();

        DataCollectionUtil.removeByPrimaryKey(m_items, m_primaryKeys);
        while (find.hasNext()) {
            assertFalse(m_items.contains(find.next()));
        }
        assertEquals(m_originalSize - m_primaryKeys.length, m_items.size());
    }

    public void testMoveUpByPrimaryKey() {
        DataCollectionUtil.moveByPrimaryKey(m_items, m_primaryKeys, -1);
        DataCollectionItem[] items = (DataCollectionItem[]) m_items
                .toArray(new DataCollectionItem[0]);
        assertEquals(new Integer(10), items[9].getPrimaryKey());
        assertEquals(new Integer(9), items[10].getPrimaryKey());
        assertEquals(new Integer(20), items[19].getPrimaryKey());
        assertEquals(new Integer(19), items[20].getPrimaryKey());
        assertEquals(new Integer(30), items[29].getPrimaryKey());
        assertEquals(new Integer(29), items[30].getPrimaryKey());
    }

    public void testMoveDownByPrimaryKey() {
        DataCollectionUtil.moveByPrimaryKey(m_items, m_primaryKeys, 1);
        DataCollectionItem[] items = (DataCollectionItem[]) m_items
                .toArray(new DataCollectionItem[0]);
        assertEquals(new Integer(10), items[11].getPrimaryKey());
        assertEquals(new Integer(11), items[10].getPrimaryKey());
        assertEquals(new Integer(20), items[21].getPrimaryKey());
        assertEquals(new Integer(21), items[20].getPrimaryKey());
        assertEquals(new Integer(30), items[31].getPrimaryKey());
        assertEquals(new Integer(31), items[30].getPrimaryKey());
    }

    public void testUpdatePositions() {
        Collections.reverse(m_items);
        DataCollectionUtil.updatePositions(m_items);
        for (int i = 0; i < m_items.size(); i++) {
            DataCollectionItem item = (DataCollectionItem) m_items.get(i);
            assertEquals(i, item.getPosition());
            assertEquals(new Integer((m_items.size() - 1) - i), item.getPrimaryKey());
        }
    }

    public void testMoveUpByPrimaryKeySqueezeHoles() {
        int keys[] = {
            0, 1, 3
        };
        DataCollectionUtil.moveByPrimaryKey(m_items, ArrayUtils.toObject(keys), -1);
        DataCollectionItem[] items = (DataCollectionItem[]) m_items
                .toArray(new DataCollectionItem[0]);
        assertEquals(new Integer(0), items[0].getPrimaryKey());
        assertEquals(new Integer(1), items[1].getPrimaryKey());
        assertEquals(new Integer(3), items[2].getPrimaryKey());
    }

    public void testMoveDownByPrimaryKeySqueezeHoles() {
        int keys[] = {
            35, 38, 39
        };
        DataCollectionUtil.moveByPrimaryKey(m_items, ArrayUtils.toObject(keys), 10);
        DataCollectionItem[] items = (DataCollectionItem[]) m_items
                .toArray(new DataCollectionItem[0]);
        assertEquals(new Integer(35), items[37].getPrimaryKey());
        assertEquals(new Integer(38), items[38].getPrimaryKey());
        assertEquals(new Integer(39), items[39].getPrimaryKey());
    }

    static class TestCollectionItem implements DataCollectionItem {

        private Integer m_id;

        private int m_position;

        TestCollectionItem(int id, int position) {
            m_id = new Integer(id);
            m_position = position;
        }

        public Object getPrimaryKey() {
            return m_id;
        }

        public int getPosition() {
            return m_position;
        }

        public void setPosition(int position) {
            m_position = position;
        }
    }
}
