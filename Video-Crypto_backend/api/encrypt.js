const router = require('express').Router();
const request = require('request');
const Config = require('../config/basic');
const executor = require('child_process').execSync;

const { Api, JsonRpc, RpcError, Numeric } = require('eosjs');
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig');      // development only
const fetch = require('node-fetch');                                    // node only; not needed in browsers
const { TextEncoder, TextDecoder } = require('util');                   // node only; native TextEncoder/Decoder
//const { TextEncoder, TextDecoder } = require('text-encoding');          // React Native, IE11, and Edge Browsers only

const privateKeys = Config.privateKeyList;

const signatureProvider = new JsSignatureProvider(privateKeys);
const rpc = new JsonRpc('http://127.0.0.1:8000', { fetch });
const api = new Api({ rpc, signatureProvider, textDecoder: new TextDecoder(), textEncoder: new TextEncoder() });


router.post('/', (req, res) => {
    const msg = {
        filename: req.body.filename,
        privatekey: req.body.privatekey
        //iv: req.body.iv
    }
    /*
     * do somework with c++.
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
        let wtf = executor(Config.relative_path + '/ffmpegDemo 1 ' + msg.filename + ' ' + msg.key + ' ' + msg.iv).toString().trim();
        let ret = {
            strongkey: '',
            weakkey: '',
            iv: '',
            filename: ''
        }
        let list = wtf.replace(/\r/g, '').replace(/ /g, '').split('\n');
        let len = list.length;
        let i = 0, status = 0, ans = 'failed';
        while(i < len) {
            if (wtf[i] === '') {
                i++;
                continue;
            }

            if (list[i] === 'strongkey') {
                i++;
                status = 1;
            } else if (list[i] === 'weakkey') {
                i++;
                status = 2;
            } else if (list[i] === 'iv'){
                i++;
                status = 3;
            } else if (list[i] === 'success') {
                ans = 'success';
            }

            if (status ===  1) {
                ret.strongkey += wtf[i];
                i++;
            } else if (status === 2) {
                ret.weakkey += wtf[i];
                i++;
            } else if (status === 3) {
                ret.iv += wtf[i];
                i++;
            }
        }

        if (ans === 'success') {
            res.send({
                status: '200',
                msg: 'OK successfully encrypted.',
                strongkey: ret.strongkey,
                weakkey: ret.weakkey,
                iv: ret.iv
            })
        } else {
            res.send({
                status: '403',
                msg: 'OK but encryption failed.'
            })
        }
    } catch(e) {
        console.log(e);
        res.send({
            status: '500',
            msg: 'something went wrong.'
        })
    }
});


module.exports = router;