Action()
{
	#include <stdio.h>
	FILE *fp;
	int counter;
	char Item[64];
	web_add_header("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36"); 
	web_add_header("Accept", "text/html,application/xhtml+xml,application/xml,image/webp,image/apng");
	web_add_header("Connection", "keep-alive"); 
	web_set_sockets_option("SSL_VERSION", "TLS1.2");

	web_url("computers", 
		"URL=http://computer-database.gatling.io/computers", 
		"Resource=0", 
		"RecContentType=text/html", 
		"Referer=", 
		"Snapshot=t1.inf", 
		"Mode=HTTP", 
		LAST);

//	lr_start_transaction("UC01_ComputerAdding");
//
//	//Операция добавления нового компьютера выполняется в цикле, количество добавляемых единиц параметризовано
//	for (counter = 1; counter <= atoi(lr_eval_string("{ComputersQuantity}")); counter++){
//		
//	//Переход на страницу добавления нового компьютера
//	web_url("Add a new computer", 
//		"URL=http://computer-database.gatling.io/computers/new", 
//		"Resource=0", 
//		"RecContentType=text/html", 
//		"Referer=http://computer-database.gatling.io/computers", 
//		"Snapshot=t4.inf", 
//		"Mode=HTTP", 
//		LAST);
//		
// 	//Формирование имени компьютера (Komus + инкремент)
//    sprintf(Item, "Komus%d", counter);
//	lr_save_string(Item,"ItemName");
//		
//	//Передача данных для добавления
//	web_submit_data("computers_2", 
//		"Action=http://computer-database.gatling.io/computers", 
//		"Method=POST", 
//		"RecContentType=text/html", 
//		"Referer=http://computer-database.gatling.io/computers/new", 
//		"Snapshot=t5.inf", 
//		"Mode=HTTP", 
//		ITEMDATA, 
//		"Name=name", "Value={ItemName}", ENDITEM, 
//		"Name=introduced", "Value={CurrentDate}", ENDITEM, 
//		"Name=discontinued", "Value={FutureDate}", ENDITEM, 
//		"Name=company", "Value={ItemNumberRnd}", ENDITEM, 
//		LAST);
//	}
//	
//	lr_end_transaction("UC01_ComputerAdding",LR_AUTO);

//	lr_think_time(15);
//
//	lr_start_transaction("UC02_QuantityFinding");
//
//	//
//	web_reg_save_param_regexp (
//    	"ParamName=SearchItemQuantity",
//    	"RegExp=<a>Displaying.*to.*of (.*)<",
//    	//"Ordinal=All",
//		LAST );
//	
//	//Запрос с параметризованым именем
//	web_submit_data("computers_3", 
//		"Action=http://computer-database.gatling.io/computers", 
//		"Method=GET", 
//		"EncType=", 
//		"RecContentType=text/html", 
//		"Referer=http://computer-database.gatling.io/computers", 
//		"Snapshot=t7.inf", 
//		"Mode=HTTP", 
//		ITEMDATA, 
//		"Name=f", "Value={QuantityFindingCriteria}", ENDITEM, 
//		LAST);
//	
//	lr_message("Quantity of requested item: %s", lr_eval_string("{SearchItemQuantity}"));
//
//	lr_end_transaction("UC02_QuantityFinding",LR_AUTO);

	lr_think_time(15);
	
	lr_start_transaction("UC03_CSVWriting");
//
//	fp = fopen(lr_eval_string("{FilePath}"),  "rb+");
	
	web_reg_save_param_ex(
	    "ParamName=cellString", 
	    "LB/IC=<td>",
	    "RB/IC=<\/td>",
	    "Ordinal=all",
	    SEARCH_FILTERS,
	        "Scope=body",
		LAST);

	
	web_submit_data("computers_4", 
		"Action=http://computer-database.gatling.io/computers", 
		"Method=GET", 
		"EncType=", 
		"RecContentType=text/html", 
		"Referer=http://computer-database.gatling.io/computers?f={QuantityFindingCriteria}", 
		"Snapshot=t8.inf", 
		"Mode=HTTP", 
		ITEMDATA, 
		"Name=f", "Value=IBM", ENDITEM, 
		LAST);
	
	//fprintf(fp, "%s;%s;%s;%s\n", lr_eval_string("{paramter_1}"), lr_eval_string("{parameter_2}"));
//	lr_think_time(6);
//
//	web_url("Next &rarr;", 
//		"URL=http://computer-database.gatling.io/computers?p=1&f=IBM", 
//		"Resource=0", 
//		"RecContentType=text/html", 
//		"Referer=http://computer-database.gatling.io/computers?f=IBM", 
//		"Snapshot=t9.inf", 
//		"Mode=HTTP", 
//		LAST);
//
	lr_end_transaction("UC03_CSVWriting",LR_AUTO);

	return 0;
}