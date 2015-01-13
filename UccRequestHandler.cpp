/*
 * UccRequestHandler.cpp
 *
 *  Created on: Jan 13, 2015
 *      Author: root
 */

#include "UccRequestHandler.h"

#include <string>

using namespace std;

UccRequestHandler::UccRequestHandler(const Wt::Http::Request& request,
		Wt::Http::Response& response) {
	this->request = request;
	this->response = response;
}

void UccRequestHandler::handleRequest() {
	locationType = "";
	const string* location = NULL;
	if (request.getParameter("point")) {
		locationType = "point";
		location = request.getParameter("point");
	} else if (request.getParameter("bounds")) {
		locationType = "bounds";
		location = request.getParameter("bounds");
	}
}
