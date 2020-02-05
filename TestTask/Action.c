Action()
{
	#include <stdio.h>
	int counter, rows, page;
	FILE *FileVarriable;  
	char Item[64]; 
	web_add_header("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36"); 
	web_add_header("Accept", "text/html,application/xhtml+xml,application/xml,image/webp,image/apng");
	web_add_header("Connection", "keep-alive"); 
	web_set_sockets_option("SSL_VERSION", "TLS1.2");

	web_url("Computers", 
		"URL=http://computer-database.gatling.io/computers", 
		"Resource=0", 
		"RecContentType=text/html", 
		"Referer=", 
		"Snapshot=t1.inf", 
		"Mode=HTTP", 
		LAST);

	lr_start_transaction("UC01_ComputerAdding");

	//Операция добавления нового компьютера выполняется в цикле, количество добавляемых единиц параметризовано
	for (counter = 1; counter <= atoi(lr_eval_string("{ComputersQuantity}")); counter++){
		
	//Переход на страницу добавления нового компьютера
	web_url("Add a new computer", 
		"URL=http://computer-database.gatling.io/computers/new", 
		"Resource=0", 
		"RecContentType=text/html", 
		"Referer=http://computer-database.gatling.io/computers", 
		"Snapshot=t2.inf", 
		"Mode=HTTP", 
		LAST);
		
 	//Формирование имени компьютера (Komus + инкремент)
    sprintf(Item, "Komus%d", counter);
	lr_save_string(Item,"ItemName");
		
	//Передача данных для добавления
	web_submit_data("Submit", 
		"Action=http://computer-database.gatling.io/computers", 
		"Method=POST", 
		"RecContentType=text/html", 
		"Referer=http://computer-database.gatling.io/computers/new", 
		"Snapshot=t3.inf", 
		"Mode=HTTP", 
		ITEMDATA, 
		"Name=name", "Value={ItemName}", ENDITEM, 
		"Name=introduced", "Value={FutureDate}", ENDITEM, // Параметр FutureDate определяется как текущая дата + 1 день
		"Name=discontinued", "Value=2020-01-01", ENDITEM, // Вместо хардкода можно использовать параметр FutureDate в этом случае и CurrentDate в предыдущем, тк по смыслу discontinued позже introduced
		"Name=company", "Value={ItemNumber}", ENDITEM, // Вместо параметра ItemNumber, соответствующего значению "RCA" можно использовать параметр ItemNumberRnd, который выбирает случайную компанию из списка
		LAST);
	}
	
	lr_end_transaction("UC01_ComputerAdding",LR_AUTO);

	lr_think_time(15);

	lr_start_transaction("UC02_QuantityFinding");

	//
	web_reg_save_param_regexp (
    	"ParamName=SearchItemQuantity",
    	"RegExp=<a>Displaying.*to.*of (.*)<",
    	//"Ordinal=All",
		LAST );
	
	//Запрос с параметризованым названием вещи
	web_submit_data("Sort by name", 
		"Action=http://computer-database.gatling.io/computers", 
		"Method=GET", 
		"EncType=", 
		"RecContentType=text/html", 
		"Referer=http://computer-database.gatling.io/computers", 
		"Snapshot=t4.inf", 
		"Mode=HTTP", 
		ITEMDATA, 
		"Name=f", "Value={QuantityFindingCriteria}", ENDITEM, 
		LAST);
	

	//Найденное значение выводится в консоль и лог 
	lr_message("Quantity of requested item: %s", lr_eval_string("{SearchItemQuantity}")); 

	lr_end_transaction("UC02_QuantityFinding",LR_AUTO);

	lr_think_time(15);
	
	lr_start_transaction("UC03_CSVWriting");
	
	/*
	***********************************
	Было бы логично искать по xpath, только икспат в вугене это одна из самых своенравных вещей, с которыми я сталкивался в нагрузке,
	поэтому старый добрый boundary extractor или регулярочка берутся за дело
	***********************************	
	*/
	web_reg_save_param_regexp (
    	"ParamName=cellString",
    	"RegExp=<td>\\s+(\\S.*)\\s*<\/td>",
    	"Ordinal=All",
		LAST );
	web_reg_save_param_regexp (
    	"ParamName=1stCol",
    	"RegExp=<td><a href\=.*>(.*)<\/a>",
    	"Ordinal=All",
		LAST );
	web_reg_save_param_regexp (
    	"ParamName=nextButton",
    	"RegExp=class\=\"next(.*)\"",
		LAST );
	
	
	web_submit_data("Sort by another name", 
		"Action=http://computer-database.gatling.io/computers", 
		"Method=GET", 
		"EncType=", 
		"RecContentType=text/html", 
		"Referer=http://computer-database.gatling.io/computers?f={InfoFindingCriteria}", 
		"Snapshot=t5.inf", 
		"Mode=HTTP", 
		ITEMDATA, 
		"Name=f", "Value=IBM", ENDITEM, 
		LAST);
	
	FileVarriable = fopen (lr_eval_string("{FilePath}"),"a");
	rows = 1;
	
	//Запись в файл первой страницы
    for (counter = 1; counter <= atoi(lr_eval_string("{cellString_count}")); counter=counter+3){
	
		//Проверка на содержание в строке <em> Если параметр содержит тег, просто заменяем его на прочерк
		if (strstr(lr_paramarr_idx("cellString", counter), "<em>")==0){
			lr_save_string(lr_paramarr_idx("cellString", counter), "B");
		}
		else lr_save_string("-", "B");
		if (strstr(lr_paramarr_idx("cellString", counter+1), "<em>")==0){
			lr_save_string(lr_paramarr_idx("cellString", counter+1), "C");
		}
		else lr_save_string("-", "C");
		if (strstr(lr_paramarr_idx("cellString", counter+2), "<em>")==0){
			lr_save_string(lr_paramarr_idx("cellString", counter+2), "D");
		}
		else lr_save_string("-", "D");
		lr_save_string(lr_paramarr_idx("1stCol", rows), "A");
		
		fprintf (FileVarriable, "%s;%s;%s;%s\n", lr_eval_string("{A}"), lr_eval_string("{B}"), lr_eval_string("{C}"), lr_eval_string("{D}"));
		rows++;
	}
	page = 1;
	
	while(strlen(lr_eval_string("{nextButton}")) < 1){
		
		lr_save_int(page, "page");
		
		web_reg_save_param_regexp (
    	"ParamName=cellString",
    	"RegExp=<td>\\s+(\\S.*)\\s*<\/td>",
    	"Ordinal=All",
		LAST );
		web_reg_save_param_regexp (
    	"ParamName=1stCol",
    	"RegExp=<td><a href\=.*>(.*)<\/a>",
    	"Ordinal=All",
		LAST );
		web_reg_save_param_regexp (
    	"ParamName=nextButton",
    	"RegExp=class\=\"next(.*)\"",
		LAST );
		
		web_url("Next page", 
			"URL=http://computer-database.gatling.io/computers?p={page}&f={InfoFindingCriteria}", 
			"Resource=0", 
			"RecContentType=text/html", 
			//"Referer=http://computer-database.gatling.io/computers?f=IBM", 
			"Snapshot=t6.inf", 
			"Mode=HTTP", 
			LAST);
		
		rows = 1;
		//Запись в файл последующих страниц
	    for (counter = 1; counter <= atoi(lr_eval_string("{cellString_count}")); counter=counter+3){
		
			//Проверка на содержание в строке <em>
			if (strstr(lr_paramarr_idx("cellString", counter), "<em>")==0){
				lr_save_string(lr_paramarr_idx("cellString", counter), "B");
			}
			else lr_save_string("-", "B");
			if (strstr(lr_paramarr_idx("cellString", counter+1), "<em>")==0){
				lr_save_string(lr_paramarr_idx("cellString", counter+1), "C");
			}
			else lr_save_string("-", "C");
			if (strstr(lr_paramarr_idx("cellString", counter+2), "<em>")==0){
				lr_save_string(lr_paramarr_idx("cellString", counter+2), "D");
			}
			else lr_save_string("-", "D");
			lr_save_string(lr_paramarr_idx("1stCol", rows), "A");
			
			fprintf (FileVarriable, "%s;%s;%s;%s\n", lr_eval_string("{A}"), lr_eval_string("{B}"), lr_eval_string("{C}"), lr_eval_string("{D}"));
			rows++;
		}
		
		page++;
	}
	
	fclose (FileVarriable);
	
	lr_end_transaction("UC03_CSVWriting",LR_AUTO);
	
	/* 
		А это спящий котик
								 |\      _,,,---,,_
							ZZZzz /,`.-'`'    -.  ;-;;,_
							     |,4-  ) )-,_. ,\ (  `'-'
							    '---''(_/--'  `-'\_)
		
		Почему он спит в моем LR скрипте? 

		Why not?
	 */

	return 0;
}