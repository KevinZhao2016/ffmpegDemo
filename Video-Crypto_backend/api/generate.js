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
        let i = 0;
        while (i < len) {
            if (wtf[i] === '\r' || wtf[i] === '' || wtf[i] === ' '){
                i++;
                continue;
            }
            if (wtf[i] === 'privateKey') {
                i++;
                while(i < len) {
                    ret.privatekey += wtf[i] + '\n';
                }
            }
            if (wtf[i] === 'publicKey') {
                i++;
                while(i < len) {
                    ret.publickey += wtf[i] + '\n';
                }
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