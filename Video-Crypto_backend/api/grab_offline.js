const router = require('express').Router();
const request = require('request');
const Config = require('../config/basic');

/*
* take signature out from a file.
* input: filename, output: signature
**/


router.post('/', (req, res) => {
    const msg = {
        filename: req.body.filename
    }
    try{
        /*
        * do some works.
        * */

        let ret = {}

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