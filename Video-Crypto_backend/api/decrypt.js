const router = require('express').Router();
const Config = require('../config/basic');
const executor = require('child_process').execSync;
///const addon = require('Video')

router.post('/', (req, res) => {
    const msg = {
        filename: req.body.filename,
        strongkey: req.body.strongkey,
        weakkey: req.body.weakkey,
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

        let wtf = executor('cd ' + Config.relative_path + ' && ./ffmpegDemo 2 ' + msg.filename + ' ' + msg.key + ' ' + msg.iv).toString();
        wtf = wtf.replace(/ /g, '').replace(/\r/g, '').replace(/\n/g, '');
        const ret = { filename: '' };
        ret.filename = 'decrypt_' + msg.filename;
        //ret.filename = msg.filename.slice(0, msg.filename.lastIndexOf('/')) + "decrypt_" + msg.filename.slice(msg.filename.lastIndexOf('/'), msg.filename.length);
        if (wtf === 'success') {
            res.send({
                status: '200',
                filename: ret.filename,
                msg: 'OK successfully decrypted'
            })
        } else {
            res.send({
                status: '403',
                msg: 'OK but failed to decrypt'
            })
        }
    } catch(e) {
        console.log(e);
        res.send({
            status: '500',
            message: 'Something went wrong.'
        })
    }
});


module.exports = router;