wget https://data.sfgov.org/resource/rqzj-sfat.csv
mongoimport -d mydb -c things --type csv --file rqzj-sfat.csv --headerline
