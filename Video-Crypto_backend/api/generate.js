const router = require('express').Router();
const request = require('request');
const Config = require('../config/basic');
const executor = require('child_process').execSync;
/*
* generate a pair of privateKey and publickey.
* input: _; output: privatekey, publickey.
* */

/*
* publickey:
* privatekey:
* */

router.post('/', (req, res) => {
    try {
        /*
        * do some works.
        * */
        let ret = {
            privatekey: "",
            publickey: ""
        };
        let tmp = ret.privatekey;
        let wtf = executor(Config.relative_path + "/ffmpegDemo 7").toString().split('\n');
        const len = wtf.length;
        let i = 0, status = 0;
        while (i < len) {
            if (wtf[i] === '\r' || wtf[i] === '' || wtf[i] === ' '){
                i++;
                continue;
            }
            if (wtf[i] === 'privateKey') {
                i++;
                status = 1;
            }
            if (wtf[i] === 'publicKey') {
                i++;
                status = 2;
            }
            if (status === 1) {
                ret.privatekey += wtf[i] + '\n';
                i++;
            } else if (status === 2) {
                ret.publickey += wtf[i] + '\n';
                i++;
            }
        }
        res.send({
            status: '200',
            privatekey: ret.privatekey,
            publickey: ret.publickey
        })
    } catch(e) {
        console.log(e);
        res.send({
            status: '500',
            msg: 'something went wrong.'
        })
    }
});


module.exports = router;