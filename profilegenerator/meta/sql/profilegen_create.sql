/*                                                          */
/* File generated by "DeZign for databases"                 */
/* Create-date    :10/24/2003                               */
/* Create-time    :5:28:26 PM                               */
/* project-name   :Not yet specified                        */
/* project-author :Not yet specified                        */
/*                                                          */

/* lower the logging level from NOTICE to WARNING */
SET client_min_messages TO warning; 

CREATE TABLE JOBS(
ID int4 NOT NULL PRIMARY KEY,
TYPE int4 NOT NULL,
STATUS varchar(1),
START_TIME varchar(40) NOT NULL,
DETAILS varchar(256),
PROGRESS varchar(256),
EXCEPTION_MESSAGE varchar(512));


CREATE TABLE MANUFACTURERS(
ID int4 NOT NULL PRIMARY KEY,
NAME varchar(20) NOT NULL);


CREATE TABLE PHONE_TYPES(
ID int4 NOT NULL PRIMARY KEY,
MODEL varchar(30) NOT NULL,
MANU_ID int4 NOT NULL);


CREATE TABLE ORGANIZATIONS(
ID int4 NOT NULL PRIMARY KEY,
NAME varchar(30) NOT NULL,
PARENT_ID int4,
DNS_DOMAIN varchar(256),
STEREOTYPE int4 NOT NULL);

CREATE UNIQUE INDEX IDX_ORGANIZATIONS_1 ON ORGANIZATIONS (DNS_DOMAIN);
CREATE UNIQUE INDEX IDX_ORGANIZATIONS_2 ON ORGANIZATIONS (NAME);

CREATE TABLE EXTENSION_POOLS(
NAME varchar(80) NOT NULL,
ORG_ID int4 NOT NULL,
ID int4 NOT NULL PRIMARY KEY);

CREATE UNIQUE INDEX IDX_EXTENSION_POOLS_1 ON EXTENSION_POOLS (ORG_ID,NAME);

CREATE TABLE EXTENSIONS(
EXTENSION_NUMBER varchar(30) NOT NULL PRIMARY KEY,
EXT_POOL_ID int4 NOT NULL,
STATUS varchar(1) NOT NULL);


CREATE TABLE REF_CONFIG_SETS(
ID int4 NOT NULL PRIMARY KEY,
NAME varchar(40) NOT NULL,
ORG_ID int4 NOT NULL);


CREATE TABLE CONFIG_SETS(
ID int4 NOT NULL PRIMARY KEY,
RCS_ID int4 NOT NULL,
PROFILE_TYPE int4 NOT NULL,
CONTENT text NOT NULL);


CREATE TABLE PHONE_GROUPS(
ID int4 NOT NULL PRIMARY KEY,
RCS_ID int4 NOT NULL,
ORG_ID int4 NOT NULL,
NAME varchar(60) NOT NULL,
PARENT_PG_ID int4);


CREATE TABLE USER_GROUPS(
ID int4 NOT NULL PRIMARY KEY,
RCS_ID int4 NOT NULL,
ORG_ID int4 NOT NULL,
NAME varchar(60) NOT NULL,
PARENT_UG_ID int4);


CREATE TABLE USERS(
ID varchar(10) NOT NULL PRIMARY KEY,
FIRST_NAME varchar(20),
ORG_ID int4 NOT NULL,
PASSWORD varchar(64),
UG_ID int4,
RCS_ID int4,
LAST_NAME varchar(30),
DISPLAY_ID varchar(80) NOT NULL,
EXTENSION varchar(30),
PROFILE_ENCRYPTION_KEY varchar(64));

CREATE UNIQUE INDEX IDX_USERS_1 ON USERS (DISPLAY_ID);

CREATE TABLE USER_APP_REF_ASSOC(
USRS_ID varchar(10) NOT NULL,
CS_ID int4);


CREATE TABLE REF_PROPERTIES(
ID int4 NOT NULL PRIMARY KEY,
CONTENT text NOT NULL,
PROFILE_TYPE int4 NOT NULL,
NAME varchar(40) NOT NULL,
CODE varchar(10) NOT NULL);

CREATE UNIQUE INDEX IDX_REF_PROPERTIES_1 ON REF_PROPERTIES (CODE);

CREATE TABLE APPLICATION_GROUPS(
ID int4 NOT NULL PRIMARY KEY,
NAME varchar(60) NOT NULL,
ORG_ID int4 NOT NULL);


CREATE TABLE USER_GROUP_AG_ASSOC(
UG_ID int4 NOT NULL,
AG_ID int4 NOT NULL,
PRIMARY KEY (UG_ID,AG_ID));


CREATE TABLE CS_PROPERTY_PERMISSIONS(
REF_PROP_ID int4 NOT NULL,
RCS_ID int4 NOT NULL,
IS_READ_ONLY int4 NOT NULL,
IS_FINAL int4 NOT NULL,
PRIMARY KEY (REF_PROP_ID,RCS_ID));


CREATE TABLE USER_CS_ASSOC(
USRS_ID varchar(10) NOT NULL,
CS_ID int4 NOT NULL,
PRIMARY KEY (USRS_ID,CS_ID));


CREATE TABLE PNG_CS_ASSOC(
CS_ID int4 NOT NULL,
PG_ID int4 NOT NULL,
PRIMARY KEY (CS_ID,PG_ID));


CREATE TABLE UG_CS_ASSOC(
UG_ID int4 NOT NULL,
CS_ID int4 NOT NULL,
PRIMARY KEY (UG_ID,CS_ID));


CREATE TABLE VERSIONS(
APPLIED date NOT NULL,
VERSION int4 NOT NULL PRIMARY KEY);


CREATE TABLE ADMIN_GROUPS(
ID varchar(20) NOT NULL PRIMARY KEY,
NAME varchar(30) NOT NULL,
ORG_ID_1 int4 NOT NULL);


CREATE TABLE USER_ADMIN_ASSOC(
USRS_ID varchar(10) NOT NULL,
ADM_GRPS_ID varchar(20) NOT NULL,
PRIMARY KEY (USRS_ID,ADM_GRPS_ID));


CREATE TABLE CORE_SOFTWARE_DETAILS(
ID int4 NOT NULL PRIMARY KEY,
INSTALLED_DATE date NOT NULL,
DESCRIPTION varchar(30),
VERSION varchar(20) NOT NULL,
NAME varchar(20) NOT NULL);


CREATE TABLE LOGICAL_PHONES(
ID int4 NOT NULL PRIMARY KEY,
SHORT_NAME varchar(60) NOT NULL,
CSD_ID int4 NOT NULL,
ORG_ID int4 NOT NULL,
RCS_ID int4 NOT NULL,
PG_ID int4,
USRS_ID varchar(10),
DESCRIPTION varchar(100),
PT_ID int4 NOT NULL,
SERIAL_NUMBER varchar(30));

CREATE UNIQUE INDEX IDX_LOGICAL_PHONES_1 ON LOGICAL_PHONES (SHORT_NAME);
CREATE UNIQUE INDEX IDX_LOGICAL_PHONES_2 ON LOGICAL_PHONES (PT_ID,SERIAL_NUMBER);

CREATE TABLE PROFILE_DATA(
LOG_PN_ID int4 NOT NULL,
TYPE int4 NOT NULL,
SEQ_NUMBER int4 NOT NULL,
PRIMARY KEY (LOG_PN_ID,TYPE));


CREATE TABLE REF_PHONE_CHARACTERISTICS(
CHARACTERISTIC varchar(30) NOT NULL PRIMARY KEY);


CREATE TABLE APPLICATIONS(
ID int4 NOT NULL PRIMARY KEY,
PT_ID int4 NOT NULL,
REF_PROP_ID int4 NOT NULL,
URL varchar(200) NOT NULL,
ORG_ID int4 NOT NULL,
NAME varchar(60) NOT NULL,
DESCRIPTION varchar(512));


CREATE TABLE PHONE_CHARACTERISTICS(
RP_CHARS_CHARACTERISTIC varchar(30) NOT NULL,
VALUE varchar(60) NOT NULL,
PT_ID int4,
APPS_ID int4);


CREATE TABLE APP_AG_ASSOC(
APPS_ID int4 NOT NULL,
AG_ID int4 NOT NULL,
PRIMARY KEY (APPS_ID,AG_ID));


CREATE TABLE USER_AG_ASSOC(
USRS_ID varchar(10) NOT NULL,
AG_ID int4 NOT NULL,
PRIMARY KEY (USRS_ID,AG_ID));


CREATE TABLE ROLES(
NAME varchar(10) NOT NULL PRIMARY KEY);


CREATE TABLE USER_ROLES(
USRS_ID varchar(10) NOT NULL,
ROLES_NAME varchar(10) NOT NULL,
PRIMARY KEY (USRS_ID,ROLES_NAME));


CREATE TABLE ALIASES(
ALIAS varchar(256) NOT NULL,
USER_ID varchar(10) NOT NULL,
ALIAS_ORDER int4 NOT NULL,
PRIMARY KEY (USER_ID,ALIAS_ORDER));


CREATE TABLE PATCHES(
PATCH_NUMBER int4 NOT NULL PRIMARY KEY,
DATE_APPLIED date NOT NULL,
STATUS varchar(1) NOT NULL,
DESCRIPTION varchar(80));


CREATE TABLE PHONE_TYPE_CSD_ASSOC(
CSD_ID int4 NOT NULL,
PT_ID int4 NOT NULL,
PRIMARY KEY (CSD_ID,PT_ID));


CREATE TABLE ADMIN_GROUP_ASSIGNMENTS(
ID int4 NOT NULL PRIMARY KEY,
ADM_GRPS_ID varchar(20) NOT NULL,
USRS_ID varchar(10),
LOG_PN_ID int4,
UG_ID int4,
PG_ID int4);


CREATE TABLE CS_PROFILE_DETAILS(
ID int4 NOT NULL PRIMARY KEY,
PROJECTION_CLASS varchar(256) NOT NULL,
CSD_ID int4 NOT NULL,
FILE_NAME varchar(30) NOT NULL,
RENDER_CLASS varchar(256) NOT NULL,
PROFILE_TYPE int4 NOT NULL,
XSLT_URL varchar(512) NOT NULL);

CREATE UNIQUE INDEX IDX_CS_PROFILE_DETAILS_1 ON CS_PROFILE_DETAILS (CSD_ID,PROFILE_TYPE);

CREATE TABLE LOG_PHONE_CS_ASSOC(
CS_ID int4 NOT NULL,
LOG_PN_ID int4 NOT NULL,
PRIMARY KEY (CS_ID,LOG_PN_ID));


/** 
 *   W A R N I N G  :   
 *   exact copy maintained in v8_mods.sql
 */

/**
 * F O L D E R
 */
create table folder(
  folder_id int4 not null primary key,
  resource varchar(256) not null,
  label varchar(256),
  parent_id int4
);

create sequence folder_seq;
create unique index idx_folder_resource_label on folder (resource, label);


/**
 * F O L D E R   S E T T I N G
 */
create table folder_setting(
  folder_id int4 not null,
  path varchar(256) not null,
  value varchar(256),
  hidden bool,
  primary key (folder_id, path)
);

/** 
 * S E T T I N G   S T O R A G E
 */
create table storage(
  storage_id int4 not null primary key
);

create sequence storage_seq;

/**
 * S E T T I N G
 */
create table setting(
  storage_id int4 not null,
  path varchar(256) not null,
  value varchar(256) not null,
  primary key (storage_id, path)
);

create sequence setting_seq;

/**
 * E N D P O I N T
 */
create table endpoint(
  endpoint_id int4 not null primary key, 
  serial_number varchar(256) not null,
  name varchar(256),
  phone_id varchar(256) not null,
  storage_id int4,
  folder_id int4 not null
);

/* 
 * would like to add FK contraint to setting table
 * but setting_id can be null and I don't think thats
 * possible w/postgres
alter table endpoint
add constraint fk_endpoint_1
foreign key (storage_id) references storage (storage_id);
 */
 
create sequence endpoint_seq;
create unique index idx_endpoint_sernum on endpoint (serial_number);

alter table endpoint
add constraint fk_endpoint_2
foreign key (folder_id) references folder (folder_id);

/* 
 * L I N E 
 */
create table line(
  line_id int4 not null primary key,
  position int4 not null,
  user_id int4 not null,
  storage_id int4,
  folder_id int4 not null,
  endpoint_id int4 not null
);
create sequence line_seq;
/*
 * relax this, otherwise adjusting positions gets messy
create unique index idx_line_pos_endpt on line (endpoint_id, position);
 */

alter table line
add constraint fk_line_1 
foreign key (endpoint_id) references endpoint (endpoint_id);

alter table line
add constraint fk_line_2
foreign key (folder_id) references folder (folder_id);

/* 
 * would like to add FK contraint to setting table
 * but setting_id can be null and I don't think thats
 * possible w/postgres
alter table line
add constraint fk_line_2
foreign key (storage_id) references storage (storage_id);
*/

/** may relax if user null ok */
alter table line
add constraint fk_line_3
foreign key (user_id) references users (id);


/** 
 *   W A R N I N G  :   
 *   exact copy maintained in v8_mods_2.sql
 */


/** 
 * R I N G
 */
create table ring(
  ring_id int4 not null primary key,
  user_id int4 not null,
  number varchar(256),
  expiration int4,
  position int4 not null,
  ring_type varchar(40)
);

create unique index idx_ring_user_ring on ring (user_id, ring_id);
create index idx_ring_user on ring (user_id);

create sequence ring_seq;

alter table ring
add constraint fk_user_1
foreign key (user_id) references users (id);


