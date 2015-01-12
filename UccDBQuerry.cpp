/*
 * UccDBQuerry.cpp
 *
 *  Created on: Jan 11, 2015
 *      Author: root
 */

#include "UccDBQuerry.h"

#include <boost/algorithm/string.hpp>

using namespace boost;
//================={ $skip : 5 }
UccDBQuerry::UccDBQuerry(string collection, string locationType,
		const string* location, const string* radius, const string* limit, const string* foodItems,
		const string* sortText, const string* name, const string* status, const string* category) {
	if (collection.size()<=0) {
		//throw Exception("badlocation!");
		cout << "================= bad collection!" << endl;
		return;
	}
	this->collection = collection;
	cout<<"============before processloc"<<endl;
	this->processLocation(locationType, location, radius);
	if (limit != NULL) {
		this->limit = atoi((*limit).c_str());
	}
	if (foodItems != NULL) {
		this->foodItems = *foodItems;
	}
	if (sortText != NULL) {
		this->sortText = atoi((*sortText).c_str());
	}
	if (name != NULL) {
		this->name = *name;
	}
	if (status != NULL) {
		this->status = *status;
	}
	if (category != NULL) {
		this->category = *category;
	}
	cout<<"============generate"<<endl;
	generateQueryCommand();
	cout<<"============generatef"<<endl;
}

void UccDBQuerry::processLocation(string locationType, const string* location,
		const string *radius) {
	if (locationType.size() > 0) {
		this->locType = locationType;
	} else {
		this->locType = "";
	}

	if (this->locType.size() > 0 && location == NULL) {
		//throw Exception("badlocation!");
		cout << "================= badlocation!" << endl;
	}

	cout<<"locType"<<this->locType<<endl;
	if (this->locType == "bounds") {
		vector<string> numbers;
		cout<<"beforesplit"<<*(location)<<endl;
		algorithm::split(numbers, *(location), is_any_of(","));
		cout<<"aftersplit"<<numbers.size()<<endl;
		if (numbers.size() != 4) {
			// throw exception
			cout << "================== bad bounds!" << endl;
		}
		this->firstLoc.push_back(atof(numbers[1].c_str()));
		this->firstLoc.push_back(atof(numbers[0].c_str()));
		this->secondLoc.push_back(atof(numbers[3].c_str()));
		this->secondLoc.push_back(atof(numbers[2].c_str()));
	} else if (this->locType == "point") {
		vector<string> numbers;
		algorithm::split(numbers, *(location), is_any_of(","));
		if (numbers.size() != 2) {
			// throw exception
			cout << "=============== bad point!" << endl;
		}
		this->firstLoc.push_back(atof(numbers[1].c_str()));
		this->firstLoc.push_back(atof(numbers[0].c_str()));
		if (radius == NULL) {
			this->secondLoc.push_back(DEFAULT_RADIUS / EARTH_RADIUS);
		} else {
			this->secondLoc.push_back(atof((*radius).c_str()) / EARTH_RADIUS);
		}
	}
}

void UccDBQuerry::generateQueryCommand() {
	BSONObjBuilder cmd_builder;
	cmd_builder.append("aggregate", this->collection);
	BSONArrayBuilder array_builder;
	BSONObjBuilder match_builder;
	BSONObjBuilder condition_builder;

	if (this->foodItems.size() > 0) {
		BSONObjBuilder search_builder;
		search_builder.append("$search", this->foodItems);
		condition_builder.append("$text", search_builder.obj());
	}
	if(this->locType.size() > 0) {
		BSONObjBuilder geo_builder;
		BSONObjBuilder scope_builder;
		BSONArrayBuilder array_builder;
		BSONArrayBuilder coordinate_builder;
		if(this->locType == "point") {
			coordinate_builder.append(this->firstLoc[0]);
			coordinate_builder.append(this->firstLoc[1]);
			array_builder.append(coordinate_builder.arr());
			array_builder.append(this->secondLoc[0]);
			scope_builder.append("$centerSphere", array_builder.arr());
		} else if(this->locType == "bounds") {
			coordinate_builder.append(this->firstLoc[0]);
			coordinate_builder.append(this->firstLoc[1]);
			array_builder.append(coordinate_builder.arr());
			BSONArrayBuilder coordinate2_builder;
			coordinate2_builder.append(this->secondLoc[0]);
			coordinate2_builder.append(this->secondLoc[1]);
			array_builder.append(coordinate2_builder.arr());
			scope_builder.append("$box", array_builder.arr());
		}
		geo_builder.append("$geoWithin", scope_builder.obj());
		condition_builder.append("loc", geo_builder.obj());
	}

	if(this->name.size() > 0) {
		condition_builder.append("Applicant", this->name);
	}
	if(this->status.size() > 0) {
		condition_builder.append("Status", this->status);
	}
	if(this->category.size() > 0) {
		condition_builder.append("FacilityType", this->status);
	}

	match_builder.append("$match", condition_builder.obj());
	array_builder.append(match_builder.obj());

	if(this->limit > 0) {
		BSONObjBuilder limit_builder;
		limit_builder.append("$limit", this->limit);
		array_builder.append(limit_builder.obj());
	}

	if(this->sortText == 1) {
		BSONObjBuilder sort_builder;
		BSONObjBuilder score_builder;
		BSONObjBuilder meta_builder;
		meta_builder.append("$meta", "textScore");
		score_builder.append("$score", meta_builder.obj());
		sort_builder.append("$sort", score_builder.obj());
		array_builder.append(sort_builder.obj());
	}
	cmd_builder.append("pipeline", array_builder.arr());
	this->command = cmd_builder.obj();
}

BSONObj& UccDBQuerry::getQueryCommand() {
	return this->command;
}
