const router = require('express').Router();
const request = require('request');
const Config = require('../config/basic');

/*
* generate a pair of privateKey and publickey.
* input: _; output: privatekey, publickey.
* */

router.post('/', (req, res) => {
    try {
        /*
        * do some works.
        * */
        let ret = {};

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