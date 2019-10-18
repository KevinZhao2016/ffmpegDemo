const router = require('express').Router();
///const addon = require('Video')

router.post('/', (req, res) => {
    const msg = {
        filename: req.body.filename,
        key: req.body.key,
        iv: req.body.iv
    }

    /*
    * do some cpp addons.
    **/
    try {
        if (Config.userName === null || Config.walletKey === null) {
            console.log('User has not login. Refused to serve.');
            res.send({
                status: 403,
                message: 'Please sign in before any other operations.'
            });
            return;
        }

        const ret = {}


        res.send({
            status: '200',
            filename: ret.filename,
            msg: 'OK successfully decrypted'
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