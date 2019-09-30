#include <iostream>
#include <string>
#include <vector>
#include <list>

#include "pugixml.hpp"

static const char* kFileName = "students2.xml";

struct Student 
{
	int id;
	std::string name;
	int age;

	Student(int id_, const std::string& name_, int age_)
		: id(id_)
		, name(name_)
		, age(age_) 
	{

	}

	void Print() const 
	{
		std::cout << "Id: " << id << ", Name: " << name << ", Age: " << age << std::endl;
	}
};

using namespace std;

// 创建XML。
static void WriteData() 
{
	// Prepare data.
	std::list<Student> students;
	students.push_back(Student(1, "MaZhe", 18));
	students.push_back(Student(2, "Miki", 19));
	students.push_back(Student(3, "Maki", 20));

	// Serialization.
	pugi::xml_document xdoc;

	pugi::xml_node xdec = xdoc.prepend_child(pugi::node_declaration);
	xdec.append_attribute("version").set_value("1.0");
	xdec.append_attribute("encoding").set_value("utf-8");

	pugi::xml_node xstudents = xdoc.append_child("Students");
	xstudents.append_child(pugi::node_comment).set_value("Students!");

	for (const Student& student : students)
	{
		pugi::xml_node xstudent = xstudents.append_child("Student");
		//xstudent.append_child(pugi::node_pcdata).set_value("123124");

		pugi::xml_attribute xstudent_id = xstudent.append_attribute("id");
		xstudent_id.set_value(student.id);

		pugi::xml_attribute xstudent_type = xstudent.append_attribute("type");
		xstudent_type.set_value("colname");

		pugi::xml_node xname = xstudent.append_child("Name");
		xname.append_child(pugi::node_pcdata).set_value(student.name.c_str());

		pugi::xml_node xage = xstudent.append_child("Age");
		char buf[128] = { 0 };
		sprintf(buf, "%d", student.age);
		xage.append_child(pugi::node_pcdata).set_value(buf);
	}

	xdoc.print(std::cout);

	xdoc.save_file(kFileName);
}

// 读取并解析XML。
static void ReadData() 
{
	std::list<Student> students;

	pugi::xml_document xdoc;
	if (!xdoc.load_file(kFileName)) 
	{
		return;
	}

	pugi::xml_node xstudents = xdoc.child("Students");
	for (pugi::xml_node xstudent = xstudents.first_child(); xstudent != NULL; xstudent = xstudent.next_sibling()) 
	{
		int id = xstudent.attribute("id").as_int();

		pugi::xml_node xname = xstudent.child("Name");
		std::string name = xname.text().as_string();

		pugi::xml_node xage = xstudent.child("Age");
		int age = xage.text().as_int();

		students.push_back(Student(id, name, age));
	}

	for (const Student& student : students) 
	{
		student.Print();
	}
}

bool inSertColToXML(char *szXmlFileName,char *bigstep,char *smallstep,const char *ColName)
{
	char filePath[256] = { 0 };
	snprintf(filePath, sizeof(filePath), "%s_stepinfo.xml", szXmlFileName);

	pugi::xml_document xmlDoc;

	if (!xmlDoc.load_file(filePath, pugi::parse_default, pugi::encoding_utf8))
	{
		cout << "No File" << endl;
		return false;
	}

	pugi::xml_node nodeThead = xmlDoc.child("rptdata").child("displaycomponent").child("thead");
	int colNum = atoi(nodeThead.attribute("cols").value());

	colNum++;

	nodeThead.attribute("cols").set_value(colNum);

	int lastColId = atoi(nodeThead.last_child().attribute("colid").value());

	pugi::xml_node nodeThead_Last = nodeThead.insert_child_before("col", nodeThead.last_child());
	nodeThead_Last.append_attribute("colid") = lastColId;

	pugi::xml_node nodeProperty = nodeThead_Last.append_child("property");

	pugi::xml_attribute xname = nodeProperty.append_attribute("name");
	xname.set_value("colname");

	pugi::xml_attribute xtype = nodeProperty.append_attribute("type");
	xtype.set_value("String");

	nodeProperty.append_child(pugi::node_pcdata).set_value(ColName);

	lastColId++;

	nodeThead.last_child().attribute("colid").set_value(lastColId);

	bool ret = xmlDoc.save_file(filePath, "\t", 1U, pugi::encoding_utf8);

	return ret;
}

bool insertRowToXML(char *szXmlFileName,char *bigstep,char * smallstep,char *MML,char *checkResult,std::vector<std::string>v,char *color)
{
	char filePath[256] = { 0 };
	snprintf(filePath, sizeof(filePath), "%s_stepinfo.xml", szXmlFileName);

	pugi::xml_document xmlDoc;

	if (!xmlDoc.load_file(filePath, pugi::parse_default, pugi::encoding_utf8))
	{
		cout << "No File" << endl;
		return false;
	}

	pugi::xml_node nodeTbody = xmlDoc.child("rptdata").child("displaycomponent").child("tbody");
	int rowNum = atoi(nodeTbody.attribute("rows").value());

	if (0 == rowNum)
	{
		rowNum++;
		nodeTbody.attribute("rows").set_value(rowNum);

		pugi::xml_node nodeRows = nodeTbody.append_child("row");
		nodeRows.append_attribute("color").set_value(color);

		pugi::xml_node nodeEntry1 = nodeRows.append_child("entry");
		nodeEntry1.append_attribute("value").set_value(szXmlFileName);

		pugi::xml_node nodeEntry2 = nodeRows.append_child("entry");
		nodeEntry2.append_attribute("value").set_value(MML);

		for (int i = 0; i < v.size(); ++i)
		{
			pugi::xml_node nodeEntry = nodeRows.append_child("entry");
			const char *entry = v[i].c_str();
			nodeEntry.append_attribute("value").set_value(entry);
		}

		pugi::xml_node nodeEntryLast = nodeRows.append_child("entry");
		nodeEntryLast.append_attribute("value").set_value(checkResult);

		bool ret = xmlDoc.save_file(filePath, "\t", 1U, pugi::encoding_utf8);

		return ret;			   		 
	}

	return true;
}

bool writeStringToCreatXML(char *szXmlFileName, char *bigstep, char *smallstep,char *MML,char *MMLAck,char*checkResult)
{
	pugi::xml_document xmlDoc;

	pugi::xml_node pre = xmlDoc.prepend_child(pugi::node_declaration);

	pre.append_attribute("version") = "1.0";
	pre.append_attribute("encoding") = "utf-8";

	pugi::xml_node nodeComment = xmlDoc.append_child(pugi::node_comment);
	nodeComment.set_value("*********Report Data********");

	pugi::xml_node nodeRptdata = xmlDoc.append_child("rptdata");
	nodeRptdata.append_attribute("bigstepid").set_value(bigstep);
	nodeRptdata.append_attribute("smallstepid").set_value(smallstep);
	nodeRptdata.append_attribute("i18nfile").set_value("upgrade_stepcfg_i18n.xml");

	pugi::xml_node nodedisplaycomponent = nodeRptdata.append_child("displaycomponent");
	nodedisplaycomponent.append_attribute("type").set_value("table");
	nodedisplaycomponent.append_attribute("title").set_value("titlelabekey1");
	nodedisplaycomponent.append_attribute("rpttype").set_value("1");
	nodedisplaycomponent.append_attribute("clearolddata").set_value("false");

	pugi::xml_node nodeCommentCol = nodedisplaycomponent.append_child(pugi::node_comment);
	nodeCommentCol.set_value("********Table Columns*********");

	pugi::xml_node nodeThead = nodedisplaycomponent.append_child("thead");
	nodeThead.append_attribute("cols").set_value(3);

	for (int i = 0; i < 3; ++i)
	{
		pugi::xml_node nodeCol = nodeThead.append_child("col");
		nodeCol.append_attribute("colid").set_value(i + 1);

		pugi::xml_node nodeProperty = nodeCol.append_child("property");

		switch (i)
		{
		case 0:
		{
			pugi::xml_attribute xname = nodeProperty.append_attribute("name");
			xname.set_value("colname");
			nodeProperty.append_child(pugi::node_pcdata).set_value("StepNum");
			break;
		}
		case 1:
		{
			pugi::xml_attribute xname = nodeProperty.append_attribute("name");
			xname.set_value("colname");
			nodeProperty.append_child(pugi::node_pcdata).set_value("MML");
			break;
		}
		case 2:
		{
			pugi::xml_attribute xname = nodeProperty.append_attribute("name");
			xname.set_value("colname");
			nodeProperty.append_child(pugi::node_pcdata).set_value("CheckResult");
			break;
		}
		default:
			break;
		}

		pugi::xml_attribute xtype = nodeProperty.append_attribute("type");
		xtype.set_value("String");

	}

	pugi::xml_node nodeCommentRow = nodedisplaycomponent.append_child(pugi::node_comment);
	nodeCommentRow.set_value("************Table Rows**************");

	pugi::xml_node nodeTbody = nodedisplaycomponent.append_child("tbody");
	nodeTbody.append_attribute("rows").set_value(0);

	char filePath[256] = { 0 };
	snprintf(filePath, sizeof(filePath), "%s_stepinfo.xml", szXmlFileName);
	bool ret = xmlDoc.save_file(filePath, "\t", 1U, pugi::encoding_utf8);

	return ret;
}

int main()
{
	/*WriteData();
	ReadData();*/

	char szXmlFileName[]="test";
	char bigstep[] ="BigStep";
	char smallstep[]="SmallStep";
	char MML[]="MML";
	char MMLAck[]="MMLAck";
	char checkResult[]="CheckResult";
	char ColName[] = "Check DDM";
	char color[] = "defalut";

	std::vector<std::string>v;
	v.clear();
	v.push_back("DISABLE");
	//v.push_back("ENABLE");

	bool b_ret1=writeStringToCreatXML(szXmlFileName, bigstep, smallstep, MML, MMLAck, checkResult);
	cout << b_ret1 << endl;

	bool b_ret2 = inSertColToXML(szXmlFileName, bigstep, smallstep, ColName);
	cout << b_ret2 << endl;

	bool b_ret3 = insertRowToXML(szXmlFileName, bigstep, smallstep, MML, checkResult, v, color);
	cout << b_ret3 << endl;

	return 0;
}