const router = require('express').Router();
const request = require('request');
const Config = require('../config/basic');
const executor = require('child_process').execSync;

/*
* certificate if the siganture is given by the publickey owner.
* input: signature, publick key. output: "YES" or "NO"
* */

router.post('/', (req, res) => {
    const msg = {
        filename: req.body.filename,
        signature: req.body.signature,
        publickey: req.body.publickey
    }
    try {
        /*
        * do some work...
        **/
        //let ans = 0;
        let answer = executor(Config.relative_path + "/ffmpegDemo 6 " + msg.signature + " " + msg.publickey)
            .toString().trim()
            .replace(/ /g, '')
            .replace(/\n/g, '')
            .replace(/\r/gs, '');

        if (answer === '1') {
            res.send({
                status: '200',
                msg: 'Yes the signature is produced by the public key'
            })
        } else if (answer === '0') {
            res.send({
                status: '403',
                msg: 'Yes but the signature is not correspond with the publickey'
            })
        }
    } catch(e) {
        console.log(e);
        res.send({
            status: '500',
            msg: 'Something went wrong.'
        })
    }
});


module.exports = router;