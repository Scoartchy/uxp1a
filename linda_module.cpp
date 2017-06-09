#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <variant>


const std::string LINDA_FILE = "test.txt";  /* change to -> "/working_dir/linda_file";*/


class Tuple
{
public:
	std::list<std::variant<int, float, std::string>> tupleElements;

	Tuple()
	{
		
	}
};

struct Element
{
	std::string type;
	std::string oper;
	std::string cond;

	Element(std::string t, std::string o, std::string c) : type(t), oper(o), cond(c) {}
};

class TuplePattern
{
public:
	std::list<Element> tuplePatternElements;
};

void output(Tuple tuple)
{
	/* Test tuple */
	tuple.tupleElements.push_back(13);
	tuple.tupleElements.push_back(0.7f);
	tuple.tupleElements.push_back("EITI");

	std::fstream file;
	file.open(LINDA_FILE.c_str(), std::ios::in | std::ios::out | std::ios::ate); 
	
	if(file.bad())
	{
		std::cout << "ERROR: The file " << LINDA_FILE.c_str() << " could not be found. Program will be terminated." << std::endl;
		std::cout << "Press any key to continue..." << std::endl;
		std::cin.get();
		exit(1);
	}
	else
	{
		std::cout << "The file " << LINDA_FILE.c_str() << " found and opened." << std::endl;
	}

	short int typeOfElement = 0;

	/* Getting tuples form list of tuples */
	for(std::variant<int, float, std::string> element : tuple.tupleElements)
	{
		/* Try get int */
		try 
		{
			std::cout << std::get<int>(element) << " ";
			++typeOfElement;
			file << std::get<int>(element) << " ";

		}
		catch (const std::bad_variant_access&) 
		{
			/* Try get float */
			try 
			{
				std::cout << std::get<float>(element) << " ";
				++typeOfElement;
				file << std::get<float>(element) << " ";
			}
			catch (const std::bad_variant_access&) /* So it must be string */
			{
				std::cout << std::get<std::string>(element) << " ";
				++typeOfElement;
				file << "\"" << std::get<std::string>(element) << "\" ";
			}
		}		
	}


	/* Go to new line */
	file << '\n';
	
	file.close();	
}

TuplePattern parse(std::string pattern)
{
	TuplePattern tuplePattern;

	int i = 0;
	
	while(i < pattern.length())
	{
		std::string type;
		while(pattern[i] != ':' && i < pattern.length())
		{
			type.push_back(pattern[i]);
			++i;
		}
			
		if(type != "integer" && type != "string" && type != "float")
		{
			std::cout << "Error in a tuple pattrern" << std::endl;
		}

		std::string oper;
		while((pattern[i] == ':' || pattern[i] == '=' || pattern[i] == '<' || pattern[i] == '>') && i < pattern.length())
		{
			oper.push_back(pattern[i]);
			++i;
		}

		std::string cond;
		while(pattern[i] != ',' && i < pattern.length())
		{
			cond.push_back(pattern[i]);
			++i;
		}

		/* Now comma and space */
		i+=2;

		Element element(type, oper, cond);
		tuplePattern.tuplePatternElements.push_back(element);

		std::cout << "Type: " << type << " Operator: " << oper << " Condition: " << cond << std::endl;
	}

	return tuplePattern;
}

void findTuple(TuplePattern tuplePattern)
{
	std::fstream file;
	file.open(LINDA_FILE.c_str(), std::ios::in | std::ios::out | std::ios::ate); 



	file.close();
}

void input(TuplePattern tuplePattern, int timeout)
{

}

void read(TuplePattern tuplePattern, int timeout)
{

}

int main()
{
	std::cout<<"UXP1A"<<std::endl;

	Tuple tuple;
	output(tuple);

	std::cout << std::endl;

	parse("integer:1, string:*, float:>5.5, string:\"abc\"");


	return 0;
}
