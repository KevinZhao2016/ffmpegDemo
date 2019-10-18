const router = require('express').Router();
const request = require('request');
const Config = require('../config/basic');

/*
* certificate if the siganture is given by the publickey owner.
* input: signature, publick key. output: "YES" or "NO"
* */

router.post('/', (req, res) => {
    const msg = {
        signature: req.signature,
        publickey: req.publickey
    }
    try {
        /*
        * do some work...
        **/

        if (answer === 1) {
            res.send({
                status: '200',
                msg: 'Yes the signature is produced by the public key'
            })
        } else if (answer === 0) {
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