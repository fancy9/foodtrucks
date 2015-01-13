/*
 * UccDBQuerry.h
 *
 *  Created on: Jan 11, 2015
 *      Author: root
 */

#ifndef UCCDBQUERRY_H_
#define UCCDBQUERRY_H_

#include <string>
#include "mongo/client/dbclient.h" // for the driver
using namespace std;
using namespace mongo;

class UccDBQuery {
	string collection;
	BSONObj command;
	string foodItems = "";
	string locType = "";
	vector<float> firstLoc;
	vector<float> secondLoc;
	int limit = DEFAULT_LIMIT;
	int offset = DEFAULT_OFFSET;
	int sortText = DEFAULT_SORTTEXT;
	string name = "";
	string status = "";
	string category = "";
	void processLocation(string locationType, const string* location, const string* radius);
	void generateQueryCommand();
public:
	static const int DEFAULT_LIMIT = 10;
	static const int DEFAULT_OFFSET = 0;
	static const int DEFAULT_SORTTEXT = 1;
	static const float DEFAULT_RADIUS = 0.5;
	static const float EARTH_RADIUS = 3959;

	UccDBQuery(string collection, string locationType, const string* location, const string* radius,
			const string* limit, const string* offset, const string* foodItems, const string* sortText,
			const string* name, const string* status, const string* category);
	BSONObj & getQueryCommand();
};

#endif /* UCCDBQUERRY_H_ */
