Action()
{

	web_url("computers", 
		"URL=http://computer-database.gatling.io/computers", 
		"Resource=0", 
		"RecContentType=text/html", 
		"Referer=", 
		"Snapshot=t1.inf", 
		"Mode=HTTP", 
		LAST);

	web_concurrent_start(NULL);

	web_url("bootstrap.min.css", 
		"URL=http://computer-database.gatling.io/assets/stylesheets/bootstrap.min.css", 
		"Resource=1", 
		"RecContentType=text/css", 
		"Referer=http://computer-database.gatling.io/computers", 
		"Snapshot=t2.inf", 
		LAST);

	web_url("main.css", 
		"URL=http://computer-database.gatling.io/assets/stylesheets/main.css", 
		"Resource=1", 
		"RecContentType=text/css", 
		"Referer=http://computer-database.gatling.io/computers", 
		"Snapshot=t3.inf", 
		LAST);

	web_concurrent_end(NULL);

	lr_think_time(38);

	lr_start_transaction("1_addComp");

	web_url("Add a new computer", 
		"URL=http://computer-database.gatling.io/computers/new", 
		"Resource=0", 
		"RecContentType=text/html", 
		"Referer=http://computer-database.gatling.io/computers", 
		"Snapshot=t4.inf", 
		"Mode=HTTP", 
		LAST);

	web_submit_data("computers_2", 
		"Action=http://computer-database.gatling.io/computers", 
		"Method=POST", 
		"RecContentType=text/html", 
		"Referer=http://computer-database.gatling.io/computers/new", 
		"Snapshot=t5.inf", 
		"Mode=HTTP", 
		ITEMDATA, 
		"Name=name", "Value=Ipad", ENDITEM, 
		"Name=introduced", "Value=2020-02-05", ENDITEM, 
		"Name=discontinued", "Value=2020-02-06", ENDITEM, 
		"Name=company", "Value=1", ENDITEM, 
		LAST);

	web_set_sockets_option("SSL_VERSION", "TLS1.2");

	web_add_header("UA-CPU", 
		"AMD64");

	web_url("suggestions.ru-RU", 
		"URL=https://ieonline.microsoft.com/iedomainsuggestions/ie11/suggestions.ru-RU", 
		"Resource=1", 
		"RecContentType=application/octet-stream", 
		"Referer=", 
		"Snapshot=t6.inf", 
		LAST);

	lr_end_transaction("1_addComp",LR_AUTO);

	lr_think_time(73);

	lr_start_transaction("1_sort");

	web_submit_data("computers_3", 
		"Action=http://computer-database.gatling.io/computers", 
		"Method=GET", 
		"EncType=", 
		"RecContentType=text/html", 
		"Referer=http://computer-database.gatling.io/computers", 
		"Snapshot=t7.inf", 
		"Mode=HTTP", 
		ITEMDATA, 
		"Name=f", "Value=ASUS", ENDITEM, 
		LAST);

	lr_end_transaction("1_sort",LR_AUTO);

	lr_think_time(33);

	lr_start_transaction("1_ibm");

	web_submit_data("computers_4", 
		"Action=http://computer-database.gatling.io/computers", 
		"Method=GET", 
		"EncType=", 
		"RecContentType=text/html", 
		"Referer=http://computer-database.gatling.io/computers?f=ASUS", 
		"Snapshot=t8.inf", 
		"Mode=HTTP", 
		ITEMDATA, 
		"Name=f", "Value=IBM", ENDITEM, 
		LAST);

	lr_think_time(6);

	web_url("Next &rarr;", 
		"URL=http://computer-database.gatling.io/computers?p=1&f=IBM", 
		"Resource=0", 
		"RecContentType=text/html", 
		"Referer=http://computer-database.gatling.io/computers?f=IBM", 
		"Snapshot=t9.inf", 
		"Mode=HTTP", 
		LAST);

	lr_end_transaction("1_ibm",LR_AUTO);

	return 0;
}