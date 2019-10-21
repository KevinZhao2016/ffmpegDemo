const router = require('express').Router();
const request = require('request');
const Config = require('../config/basic');
const executor = require('child_process').execSync;
/*
* take signature out from a file.
* input: filename, output: signature
**/

router.all('*', function (req, res, next) {
    res.header('Access-Control-Allow-Origin', '*');
    res.header('Access-Control-Allow-Methods', 'PUT, GET, POST, DELETE, OPTIONS');
    res.header('Access-Control-Allow-Headers', 'Content-Type');
    next();
});

router.post('/', (req, res) => {
    const msg = {
        filename: req.body.filename
    }
    try{
        /*
        * do some works.
        * */

        let ret = { siganture: "" };
        ret.siganture = executor("cd " + Config.relative_path + " && ./ffmpegDemo 5 " + msg.filename).toString();

        res.send({
            status: '200',
            siganture: ret.siganture,
            msg: 'OK get the signature of the video'
        })
    } catch(e) {
        console.log(e);
        res.send({
            status: '500',
            message: 'Something went wrong.'
        })

    }
});


module.exports = router;