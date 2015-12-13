#!/usr/bin/env node

function processArgs() {
    var ret = {};
    var last;
    var argv = process.argv.slice(2);
    for (var v in argv) {
        var curr = argv[v];
        if (curr.indexOf("--")==0) {
            last = curr;
        } else {
            ret[last] = curr;
        }
    }
    return ret;
}

function Communication(doStuff) {
    var argsMap = processArgs();
    var path = argsMap["--server"];

    var net = require('net');
    var client = new net.Socket();
    client.connect({path: path}, doStuff)
    client.on('end', function() { console.warn('disconnected from server'); });

    this.send = function(obj) { client.write(JSON.stringify(obj)+'\n'); };
    this.data = JSON.parse(argsMap["--data"]);

    this.setError = function(code, errorText) {
        console.warn('ERROR [%s]', errorText);
        this.send( { error: code, errorText: errorText } );
        process.exit(1);
    };

    this.setOutput = function(data) {
        this.send(data);
        process.exit(0);
    };
}

//-------------------

var comm = new Communication(main);

//Callback functions
var error = function (err, response, body) {
    comm.setError(1, JSON.stringify(err));
};
var postSuccess = function (dataString) {
    var data = JSON.parse(dataString);
    var outputUrl = ("https://twitter.com/"+data.user.screen_name+"/status/"+data.id_str);
    comm.setOutput(comm.send({percent: 100, output: {url: outputUrl} }));
};

var Twitter = require('twitter-node-client').Twitter;

var config = {
    consumerKey: comm.data.consumerKey,
    consumerSecret: comm.data.consumerSecret,
    accessToken: comm.data.accessToken,
    accessTokenSecret: comm.data.accessTokenSecret,
    callBackUrl: ""
}

for(var v in config) {
    if (v!="callBackUrl" && config[v]=="") {
        console.log("auth...", config);
        comm.setError(2, "Missing auth information");
    }
}

var twitter = new Twitter(config);

var message = comm.data.tweetText;
var inputUrls = comm.data.urls;
var uploaded = [];

function postIfDone()
{
    if (uploaded.length === inputUrls.length) {
        comm.send({percent: 90});
        twitter.postTweet({
                status: message,
                media_ids: uploaded
            }
            , error
            , postSuccess
        );
    }
}

function mediaUploaded(dataString) {
    var data = JSON.parse(dataString)
    uploaded.push(data.media_id_string);
    comm.send({percent: 60});
    postIfDone();
};

function readUrl(url) {
    var spawn = require('child_process').spawn;
    var kiocat = spawn("kioclient5", ["cat", url], { stdio: ['ignore', 'pipe', 'ignore']});

    var contents = []; //array of buffers
    kiocat.stdout.on('data', function (data) { contents.push(data); });

    kiocat.on('exit', function (exitCode) {
        if (exitCode!=0) {
            setError(exitCode, "couldn't read: " + url);
            return;
        }
        comm.send({percent: 20});

        var allData = Buffer.concat(contents);
        twitter.postMedia(
            { media_data: allData.toString("base64") }
            , error
            , mediaUploaded
        );
    });
}

function main()
{
    console.log("Connected!");

    for (var v in inputUrls) {
        readUrl(inputUrls[v])
    }
    postIfDone();
}
