/*
 +----------------------------------------------------------------------+
 | PHP Version 5                                                        |
 +----------------------------------------------------------------------+
 | Copyright (c) 1997-2013 The PHP Group                                |
 +----------------------------------------------------------------------+
 | This source file is subject to version 3.01 of the PHP license,      |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.php.net/license/3_01.txt                                  |
 | If you did not receive a copy of the PHP license and are unable to   |
 | obtain it through the world-wide-web, please send a note to          |
 | license@php.net so we can mail you a copy immediately.               |
 +----------------------------------------------------------------------+
 | Author:                                                              |
 +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/sha1.h"
#include "ext/standard/md5.h"
#include "php_nbphp.h"

#define COMMON (Z_ISREF_PP(struc) ? "&" : "")

/* If you declare any globals in php_nbphp.h uncomment this:
 ZEND_DECLARE_MODULE_GLOBALS(nbphp)
 */

/* True global resources - no need for thread safety here */
static int le_nbphp;

static FILE *fd = NULL;



/* {{{ nbphp_functions[]
 *
 * Every user visible function must have an entry in nbphp_functions[].
 */
const zend_function_entry nbphp_functions[] = {
	PHP_FE(confirm_nbphp_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(nl,	NULL)
	PHP_FE(md6,	NULL)
	PHP_FE_END	/* Must be the last line in nbphp_functions[] */
};
/* }}} */

/* {{{ nbphp_module_entry
 */
zend_module_entry nbphp_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"nbphp",
	nbphp_functions,
	PHP_MINIT(nbphp),
	PHP_MSHUTDOWN(nbphp),
	PHP_RINIT(nbphp),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(nbphp),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(nbphp),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_NBPHP_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_NBPHP
ZEND_GET_MODULE(nbphp)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
 PHP_INI_BEGIN()
 STD_PHP_INI_ENTRY("nbphp.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_nbphp_globals, nbphp_globals)
 STD_PHP_INI_ENTRY("nbphp.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_nbphp_globals, nbphp_globals)
 PHP_INI_END()
 */
/* }}} */

/* {{{ php_nbphp_init_globals
 */
/* Uncomment this function if you have INI entries
 static void php_nbphp_init_globals(zend_nbphp_globals *nbphp_globals)
 {
 nbphp_globals->global_value = 0;
 nbphp_globals->global_string = NULL;
 }
 */
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(nbphp)
{
	/* If you have INI entries, uncomment these lines
     REGISTER_INI_ENTRIES();
     */
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(nbphp)
{
	/* uncomment this line if you have INI entries
     UNREGISTER_INI_ENTRIES();
     */
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(nbphp)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(nbphp)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(nbphp)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "nbphp support", "enabled");
	php_info_print_table_end();
    
	/* Remove comments if you have entries in php.ini
     DISPLAY_INI_ENTRIES();
     */
}
/* }}} */


/* Remove the following function when you have successfully modified config.m4
 so that your module can be compiled into PHP, it exists only for testing
 purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_nbphp_compiled(string arg)
 Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_nbphp_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;
    
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}
    
	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "nbphp", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
 unfold functions in source code. See the corresponding marks just before
 function definition, where the functions purpose is also documented. Please
 follow this convention for the convenience of others editing your code.
 */

static int php_array_element_dump(zval **zv TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	int level;

 	level = va_arg(args, int);

 	if (hash_key->nKeyLength == 0) { /* numeric key */
 		fprintf(fd, "%*c[%ld]=>", level + 1, ' ', hash_key->h);
 	} else { /* string key */
 		fprintf(fd, "%*c[\"", level + 1, ' ');
 		fputs(hash_key->arKey,fd);
 		fputs("\"]=>",fd);
 	}
 	php_var_dump_new(zv, level + 2 TSRMLS_CC);
 	return 0;
}

static int php_object_property_dump(zval **zv TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	int level;
	const char *prop_name, *class_name;

	level = va_arg(args, int);

	if (hash_key->nKeyLength == 0) { /* numeric key */
		fprintf(fd, "%*c[%ld]=>", level + 1, ' ', hash_key->h);
	} else { /* string key */
		int unmangle = zend_unmangle_property_name(hash_key->arKey, hash_key->nKeyLength - 1, &class_name, &prop_name);
		fprintf(fd, "%*c[", level + 1, ' ');

		if (class_name && unmangle == SUCCESS) {
			if (class_name[0] == '*') {
				fprintf(fd, "\"%s\":protected", prop_name);
			} else {
				fprintf(fd, "\"%s\":\"%s\":private", prop_name, class_name);
			}
		} else {
			fputs("\"",fd);
			fputs(hash_key->arKey,fd);
			fputs("\"",fd);
		}
		fputs("]=>",fd);
	}
	php_var_dump_new(zv, level + 2 TSRMLS_CC);
	return 0;
}

PHPAPI void php_var_dump_new(zval **struc, int level TSRMLS_DC) /* {{{ */
{

	HashTable *myht;
	const char *class_name;
	zend_uint class_name_len;
	int (*php_element_dump_func)(zval** TSRMLS_DC, int, va_list, zend_hash_key*);
	int is_temp;

	if (level > 1) {
		fprintf(fd, "%*c", level - 1, ' ');
	}

	switch (Z_TYPE_PP(struc)) {
	case IS_BOOL:
		fprintf(fd, "%sbool(%s)\n", COMMON, Z_LVAL_PP(struc) ? "true" : "false");
		break;
	case IS_NULL:
		fprintf(fd, "%sNULL\n", COMMON);
		break;
	case IS_LONG:
		fprintf(fd, "%sint(%ld)\n", COMMON, Z_LVAL_PP(struc));
		break;
	case IS_DOUBLE:
		fprintf(fd, "%sfloat(%.*G)\n", COMMON, (int) EG(precision), Z_DVAL_PP(struc));
		break;
	case IS_STRING:
		fprintf(fd, "%sstring(%d) \"", COMMON, Z_STRLEN_PP(struc));
		fputs(Z_STRVAL_PP(struc),fd);
		fputs("\"\n",fd);
		break;
	case IS_ARRAY:
		myht = Z_ARRVAL_PP(struc);
		if (++myht->nApplyCount > 1) {
			fputs("*RECURSION*\n",fd);
			--myht->nApplyCount;
			return;
		}
		fprintf(fd, "%sarray(%d) {\n", COMMON, zend_hash_num_elements(myht));
		php_element_dump_func = php_array_element_dump;
		is_temp = 0;
		goto head_done;
	case IS_OBJECT:
		myht = Z_OBJDEBUG_PP(struc, is_temp);
		if (myht && ++myht->nApplyCount > 1) {
			fputs("*RECURSION*\n",fd);
			--myht->nApplyCount;
			return;
		}

		if (Z_OBJ_HANDLER(**struc, get_class_name)) {
			Z_OBJ_HANDLER(**struc, get_class_name)(*struc, &class_name, &class_name_len, 0 TSRMLS_CC);
			fprintf(fd, "%sobject(%s)#%d (%d) {\n", COMMON, class_name, Z_OBJ_HANDLE_PP(struc), myht ? zend_hash_num_elements(myht) : 0);
			efree((char*)class_name);
		} else {
			fprintf(fd, "%sobject(unknown class)#%d (%d) {\n", COMMON, Z_OBJ_HANDLE_PP(struc), myht ? zend_hash_num_elements(myht) : 0);
		}
		php_element_dump_func = php_object_property_dump;
head_done:
		if (myht) {
			zend_hash_apply_with_arguments(myht TSRMLS_CC, (apply_func_args_t) php_element_dump_func, 1, level);
			--myht->nApplyCount;
			if (is_temp) {
				zend_hash_destroy(myht);
				efree(myht);
			}
		}
		if (level > 1) {
			fprintf(fd, "%*c", level-1, ' ');
		}
		fputs("}\n",fd);
		break;
	case IS_RESOURCE: {
		const char *type_name = zend_rsrc_list_get_rsrc_type(Z_LVAL_PP(struc) TSRMLS_CC);
		fprintf(fd, "%sresource(%ld) of type (%s)\n", COMMON, Z_LVAL_PP(struc), type_name ? type_name : "Unknown");
		break;
	}
	default:
		fprintf(fd, "%sUNKNOWN:0\n", COMMON);
		break;
	}
}

/* {{{ proto string nbphp_trace(string str, string str, string str)
 */
PHP_FUNCTION(nl)
{
    //PHP参数和长度声明
	zval ***args;
	int argc;

    //校验PHP参数是否正确
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE) {
        return;
    }

    char *cat = NULL;
    char *path = NULL;


   	if(argc == 2)
   	    cat = Z_STRVAL_PP(args[1]);
   	else if(argc == 3)
   	{
   	    cat = Z_STRVAL_PP(args[1]);
   	    path = Z_STRVAL_PP(args[2]);
   	}

    //日志前缀
    char prefix[8] = {0};
    strcat(prefix,"[nbphp]");
    char fullprefix[256] = {0};

    //文件存储路径
    char fullpath[256] = {0};

    if(cat != NULL)
    {
        //日志前缀
        sprintf(fullprefix,"%s [%s]",prefix,cat);
    }
    else
    {
        //日志前缀
        sprintf(fullprefix,"%s [%s]",prefix,"trace");
    }

    //日志默认值
    char filename[]= "nbphp";

    //时间
    char tmp[256] = {0};
    char tmp_s[256] = {0};
    time_t t = time(0);

    strftime(tmp, sizeof(tmp), "[%Y-%m-%d %H:%M:%S]",localtime(&t));
    strftime(tmp_s, sizeof(tmp_s), "_%Y_%m.log",localtime(&t));

    if(path != NULL)
    {
        strcat(fullpath,path);
        strcat(fullpath,tmp_s);
        fd = fopen(fullpath,"a");
        fprintf(fd,"%s %s ", fullprefix, tmp);

        if(Z_TYPE_PP(args[0]) == IS_ARRAY || Z_TYPE_PP(args[0]) == IS_OBJECT)
        fputs("\n",fd);
        php_var_dump_new(args[0], 1 TSRMLS_CC);
    }
    else
    {
        strcat(fullpath,filename);
        strcat(fullpath,tmp_s);
        fd = fopen(fullpath,"a");
        fprintf(fd,"%s %s ", fullprefix, tmp);
        if(Z_TYPE_PP(args[0]) == IS_ARRAY || Z_TYPE_PP(args[0]) == IS_OBJECT)
        fputs("\n",fd);
        php_var_dump_new(args[0], 1 TSRMLS_CC);
    }
    fclose(fd);
    efree(args);
    RETURN_LONG(1);
}
/* }}} */

PHP_FUNCTION(md6)
{
    //PHP参数和长度声明
	char *arg = NULL;
	int arg_len = 0;
    //salt初始化
	char *salt = NULL;
	int salt_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &arg, &arg_len, &salt, &salt_len) == FAILURE) {
		return;
	}

    //初始化md5参数
    char md5str[33];
    PHP_MD5_CTX context_md5;
    unsigned char digest_md5[16];

    //初始化sha1参数
	char sha1str[41];
    PHP_SHA1_CTX context_sha1;
    unsigned char digest_sha1[20];

    //初始化md5参数
    char *md6str = NULL;
    int md6len = 0;

    //给md6分配存储内存
    md6len = arg_len + salt_len;
    md6str = (char *)emalloc(md6len);

    //md5赋值
    if(salt != NULL)
        sprintf(md6str,"%s%s",arg,salt);
    else
        sprintf(md6str,"%s",arg);

	//第一层sha1+salt加密
    sha1str[0] = '\0';
    PHP_SHA1Init(&context_sha1);
    PHP_SHA1Update(&context_sha1, (unsigned char *)md6str, md6len);
    PHP_SHA1Final(digest_sha1, &context_sha1);
    make_digest_ex(sha1str, digest_sha1, 20);

    //第二层md5加密
	md5str[0] = '\0';
	PHP_MD5Init(&context_md5);
	PHP_MD5Update(&context_md5, sha1str, 40);
	PHP_MD5Final(digest_md5, &context_md5);
	make_digest_ex(md5str, digest_md5, 16);

    //释放md6的内存
	efree(md6str);
	//返回md6
	RETVAL_STRING(md5str, 1);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
