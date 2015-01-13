#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WResource>
#include <Wt/WServer>
#include <Wt/Http/Response>
#include <Wt/Http/Client>
#include <Wt/WGoogleMap>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>

#include <Wt/WCheckBox>
#include <Wt/WComboBox>
#include <Wt/WContainerWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WGoogleMap>
#include <Wt/WPushButton>
#include <Wt/WStringListModel>
#include <Wt/WTemplate>

#include <jsoncpp/json/json.h>

#include <iostream>
#include <string>
#include "UccDB.h"
#include "UccDBQuery.h"

using namespace std;

class MyResource: public Wt::WResource {
public:
	UccDB * mydb;
	MyResource(Wt::WObject *parent = 0) :
			Wt::WResource(parent) {
		mydb = new UccDB("localhost", "mydb");
	}
	~MyResource() {
		delete mydb;
		beingDeleted();
	}

	void handleHttpResponse(Wt::Http::Client *client,
			boost::system::error_code err,
			const Wt::Http::Message& response) const {
		if (!err && response.status() == 200) {
			string jsonString = response.body();
			cout << jsonString << endl;
			Json::Value root;   // will contains the root value after parsing.
			Json::Reader reader;
			bool parsingSuccessful = reader.parse(jsonString, root);
			if (parsingSuccessful) {
				//	for (int i = 0; i < root.size(); i++) {
				//		cout << root[i].asString() << endl;
				//	}
			} else {
				cout << "invalid json" << endl;
			}
		}
	}

	void handleRequest(const Wt::Http::Request& request,
			Wt::Http::Response& response) {
		response.addHeader("Content-Type", "application/json; charset=utf-8");
		response.addHeader("Access-Control-Allow-Origin", "*");
		response.setMimeType("application/json");

		std::string address = "";
		if (request.getParameter("near") != NULL) {
			address = *(request.getParameter("near"));
		}

		if (address != NULL && address.size() > 0) {
			Wt::Http::Client *client = new Wt::Http::Client(this);
			client->setTimeout(15);
			client->setMaximumResponseSize(10 * 1024);
			client->done().connect(
					boost::bind(&MyResource::handleHttpResponse, this, client,
							_1, _2));
			cout << "=================" << endl;
			if (client->get(
					"http://maps.googleapis.com/maps/api/geocode/json?"
							"address=1600+Amphitheatre+Parkway,+Mountain+View,+CA&sensor=false")) {
			} else {
				//in case of an error in the %URL
			}
		}

		try {
			string locationType = "";
			const string* location = NULL;
			if (request.getParameter("point")) {
				locationType = "point";
				location = request.getParameter("point");
			} else if (request.getParameter("bounds")) {
				locationType = "bounds";
				location = request.getParameter("bounds");
			}
			cout << locationType << endl;
			const string * radius = request.getParameter("radius");
			const string * limit = request.getParameter("limit");
			const string * offset = request.getParameter("offset");
			const string * foodItems = request.getParameter("food");
			const string * sortText = request.getParameter("sort");
			const string * name = request.getParameter("name");
			const string * category = request.getParameter("category");
			const string * status = request.getParameter("status");

			UccDBQuery query("things", locationType, location, radius, limit, offset,
					foodItems, sortText, name, status, category);
			BSONObj result;
			cout << query.getQueryCommand().jsonString() << endl;
			mydb->runCommand(query.getQueryCommand(), result);

			string data = result.jsonString();
			cout << data.size() << endl;
			response.out() << data;

		} catch (const mongo::DBException &e) {
			std::cout << "caught " << e.what() << std::endl;
		}
	}

};

class GoogleMapExample: public Wt::WContainerWidget {
public:

	GoogleMapExample(Wt::WContainerWidget *parent = 0) :
			Wt::WContainerWidget(parent) {
		Wt::WHBoxLayout *layout = new Wt::WHBoxLayout();
		setLayout(layout);

		setWidth(500);
		setHeight(400);

		map_ = new Wt::WGoogleMap(Wt::WGoogleMap::Version3);
		layout->addWidget(map_, 1);

		map_->setMapTypeControl(Wt::WGoogleMap::DefaultControl);
		map_->enableScrollWheelZoom();

		mapTypeModel_ = new Wt::WStringListModel(this);
		addMapTypeControl("No control", Wt::WGoogleMap::NoControl);
		addMapTypeControl("Default", Wt::WGoogleMap::DefaultControl);
		addMapTypeControl("Menu", Wt::WGoogleMap::MenuControl);
		if (map_->apiVersion() == Wt::WGoogleMap::Version2)
			addMapTypeControl("Hierarchical",
					Wt::WGoogleMap::HierarchicalControl);
		if (map_->apiVersion() == Wt::WGoogleMap::Version3)
			addMapTypeControl("Horizontal bar",
					Wt::WGoogleMap::HorizontalBarControl);

		map_->enableDragging();

		map_->disableDoubleClickZoom();

		map_->enableScrollWheelZoom();

		map_->addMarker(Wt::WGoogleMap::Coordinate(50.885069, 4.71958));


	}
	Wt::WGoogleMap * getMap() {
			return this->map_;
    }

private:
	void panToEmWeb() {
		map_->panTo(Wt::WGoogleMap::Coordinate(50.9082, 4.66056));
	}

	void savePosition() {
		returnToPosition_->setEnabled(true);
		map_->savePosition();
	}

	void addMapTypeControl(const Wt::WString &description,
			Wt::WGoogleMap::MapTypeControl value) {
		int r = mapTypeModel_->rowCount();
		mapTypeModel_->insertRow(r);
		mapTypeModel_->setData(r, 0, description);
		mapTypeModel_->setData(r, 0, value, Wt::UserRole);
	}

	void setMapTypeControl(int row) {
		boost::any mtc = mapTypeModel_->data(row, 0, Wt::UserRole);
		map_->setMapTypeControl(
				boost::any_cast<Wt::WGoogleMap::MapTypeControl>(mtc));
	}

	void googleMapDoubleClicked(Wt::WGoogleMap::Coordinate c) {
		std::cerr << "Double clicked at coordinate (" << c.latitude() << ","
				<< c.longitude() << ")";
	}

	void googleMapClicked(Wt::WGoogleMap::Coordinate c) {
		std::cerr << "Clicked at coordinate (" << c.latitude() << ","
				<< c.longitude() << ")";
	}

private:
	Wt::WGoogleMap *map_;
	Wt::WAbstractItemModel *mapTypeModel_;

	Wt::WPushButton *returnToPosition_;
};

class HelloApplication: public Wt::WApplication {
public:
	HelloApplication(const Wt::WEnvironment& env);

private:
	Wt::WLineEdit *nameEdit_;
	Wt::WText *greeting_;
	GoogleMapExample *map;

	void greet();
};

HelloApplication::HelloApplication(const Wt::WEnvironment& env) :
		Wt::WApplication(env) {
	setTitle("Hello world");                               // application title

	root()->addWidget(new Wt::WText("What food do you want? ")); // show some text
	nameEdit_ = new Wt::WLineEdit(root());                   // allow text input
	nameEdit_->setFocus();                                 // give focus

	Wt::WPushButton *button = new Wt::WPushButton("Show me.", root()); // create a button
	button->setMargin(5, Wt::Left);                       // add 5 pixels margin

	root()->addWidget(new Wt::WBreak());                  // insert a line break

	greeting_ = new Wt::WText(root());                         // empty text

	/*
	 * Connect signals with slots
	 *
	 * - simple Wt-way
	 */
	button->clicked().connect(this, &HelloApplication::greet);

	/*
	 * - using an arbitrary function object (binding values with boost::bind())
	 */
	nameEdit_->enterPressed().connect(
			boost::bind(&HelloApplication::greet, this));

	map = new GoogleMapExample();
	root()->addWidget(map);
}

void HelloApplication::greet() {
	/*
	 * Update the text, using text input into the nameEdit_ field.
	 */
	UccDB mydb("localhost", "mydb");

	string col = "things";
	string lt = "";
	string limit = "10";
	string offset = "0";
	string food = nameEdit_->text().toUTF8();
	UccDBQuery query(col, lt, NULL, NULL, &limit, &offset,
			&food, NULL, NULL, NULL, NULL);
	BSONObj result;
	cout << query.getQueryCommand().jsonString() << endl;
	mydb.runCommand(query.getQueryCommand(), result);

	int retsz = result["result"].Array().size();
	cout<<retsz<<endl;
	for(int i=0; i<retsz; i++) {
		double lat = result["result"].Array()[i]["Latitude"].Double();
		double lng = result["result"].Array()[i]["Longitude"].Double();

		cout<<lat<<endl;
		cout<<lng<<endl;
		//int latf = atof(lat.c_str());
		//int lngf = atof(lng.c_str());
		if(i==0) {
		map->getMap()->setCenter(Wt::WGoogleMap::Coordinate(lat,lng));
		}
		map->getMap()->addMarker(Wt::WGoogleMap::Coordinate(lat,lng));
	}
	string data = result.jsonString();
	greeting_->setText(data);
}

Wt::WApplication *createApplication(const Wt::WEnvironment& env) {
	/*
	 * You could read information from the environment to decide whether
	 * the user has permission to start a new application
	 */
	return new HelloApplication(env);
}

int main(int argc, char **argv) {
	//DBtest dbt;
	//dbt.start();
	try {
		Wt::WServer server(argv[0]);

		server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);

		MyResource resource;
		server.addResource(&resource, "/test");

		server.addEntryPoint(Wt::Application, &createApplication);
		if (server.start()) {
			int sig = Wt::WServer::waitForShutdown(argv[0]);

			std::cerr << "Shutdown (signal = " << sig << ")" << std::endl;
			server.stop();
		}
	} catch (Wt::WServer::Exception& e) {
		std::cerr << e.what() << "\n";
		return 1;
	} catch (std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
		return 1;
	}
}
