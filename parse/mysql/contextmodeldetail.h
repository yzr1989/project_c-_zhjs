

sql_create_6(contextmodeldetail,
	1, 6, // The meaning of these values is covered in the user manual
	mysqlpp::sql_int, detailid,	
	mysqlpp::Time, bgtime,
	mysqlpp::Time, edtime,
	mysqlpp::sql_char, ctrltype,
	mysqlpp::sql_int, light_on,
	mysqlpp::sql_int, light_off)