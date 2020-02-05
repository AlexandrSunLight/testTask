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

	//�������� ���������� ������ ���������� ����������� � �����, ���������� ����������� ������ ���������������
	for (counter = 1; counter <= atoi(lr_eval_string("{ComputersQuantity}")); counter++){
		
	//������� �� �������� ���������� ������ ����������
	web_url("Add a new computer", 
		"URL=http://computer-database.gatling.io/computers/new", 
		"Resource=0", 
		"RecContentType=text/html", 
		"Referer=http://computer-database.gatling.io/computers", 
		"Snapshot=t2.inf", 
		"Mode=HTTP", 
		LAST);
		
 	//������������ ����� ���������� (Komus + ���������)
    sprintf(Item, "Komus%d", counter);
	lr_save_string(Item,"ItemName");
		
	//�������� ������ ��� ����������
	web_submit_data("Submit", 
		"Action=http://computer-database.gatling.io/computers", 
		"Method=POST", 
		"RecContentType=text/html", 
		"Referer=http://computer-database.gatling.io/computers/new", 
		"Snapshot=t3.inf", 
		"Mode=HTTP", 
		ITEMDATA, 
		"Name=name", "Value={ItemName}", ENDITEM, 
		"Name=introduced", "Value={FutureDate}", ENDITEM, // �������� FutureDate ������������ ��� ������� ���� + 1 ����
		"Name=discontinued", "Value=2020-01-01", ENDITEM, // ������ �������� ����� ������������ �������� FutureDate � ���� ������ � CurrentDate � ����������, �� �� ������ discontinued ����� introduced
		"Name=company", "Value={ItemNumber}", ENDITEM, // ������ ��������� ItemNumber, ���������������� �������� "RCA" ����� ������������ �������� ItemNumberRnd, ������� �������� ��������� �������� �� ������
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
	
	//������ � ���������������� ��������� ����
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
	

	//��������� �������� ��������� � ������� � ��� 
	lr_message("Quantity of requested item: %s", lr_eval_string("{SearchItemQuantity}")); 

	lr_end_transaction("UC02_QuantityFinding",LR_AUTO);

	lr_think_time(15);
	
	lr_start_transaction("UC03_CSVWriting");
	
	/*
	***********************************
	���� �� ������� ������ �� xpath, ������ ������ � ������ ��� ���� �� ����� ����������� �����, � �������� � ����������� � ��������,
	������� ������ ������ boundary extractor ��� ����������� ������� �� ����
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
	
	//������ � ���� ������ ��������
    for (counter = 1; counter <= atoi(lr_eval_string("{cellString_count}")); counter=counter+3){
	
		//�������� �� ���������� � ������ <em> ���� �������� �������� ���, ������ �������� ��� �� �������
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
		//������ � ���� ����������� �������
	    for (counter = 1; counter <= atoi(lr_eval_string("{cellString_count}")); counter=counter+3){
		
			//�������� �� ���������� � ������ <em>
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
		� ��� ������ �����
								 |\      _,,,---,,_
							ZZZzz /,`.-'`'    -.  ;-;;,_
							     |,4-  ) )-,_. ,\ (  `'-'
							    '---''(_/--'  `-'\_)
		
		������ �� ���� � ���� LR �������? 

		Why not?
	 */

	return 0;
}