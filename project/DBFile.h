#ifndef DBFILE_H
#define DBFILE_H

#include <string>

#include "Config.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"

using namespace std;


class DBFile {
private:
	File file;
	
        
	Page page;
	int pageCount;
	FileType ftype;

public:
        string fileName;          
        bool isOpen;
	DBFile ();
	virtual ~DBFile ();
	DBFile(const DBFile& _copyMe);
	DBFile& operator=(const DBFile& _copyMe);

	int Create (char* fpath, FileType file_type);
	int Open (char* fpath);
	int Close ();

	void Load (Schema& _schema, char* textFile);

	void MoveFirst ();
	void AppendRecord (Record& _addMe);
	int GetNext (Record& _fetchMe);
<<<<<<< HEAD
=======
        int GetSpecificRecord(int pNumber, int rNumber, Record& rec);
>>>>>>> Daniel
};

#endif //DBFILE_H