//Callback functions
var error = function (err, response, body) {
    console.log('ERROR [%s]', JSON.stringify(err), response.read(), body);
};
var success = function (data) {
    console.log('Data [%s]', data);
};

var Twitter = require('twitter-js-client').Twitter;

//Get this data from your twitter apps dashboard
var config = {
    "consumerKey": "MtDziLOm73Cnrbw0SHmj7ypir",
    "consumerSecret": "thUubm5krbeYRGgjPcHK7C06BZAMUZhPeRsfE81nJBwkskiuQq",
    "accessToken": "4432149885-lPmvLW3A1WPXzhfUKacuyoF8AIPA0rHCsXQoJST",
    "accessTokenSecret": "ipg1fSDPm5Sld38s7WrtFbfShf9wkDb4ihR4gD8wrBBz4",
    "callBackUrl": ""
}

var twitter = new Twitter(config);

//Example calls

twitter.getUserTimeline({ screen_name: 'AleixPol', count: '10'}, error, success);

