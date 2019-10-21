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
        //privatekey: req.body.privatekey
        //iv: req.body.iv
    };
    let accountInfo = {};
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
        const filename = 'encrypt_' + msg.filename;
        let wtf = executor('cd ' + Config.relative_path + ' && ./ffmpegDemo 1 ' + msg.filename + ' ' + filename).toString().trim();
        let ret = {
            strongkey: '',
            weakkey: '',
            iv: '',
            publickey: '',
            privatekey: '',
            signature: '',
            filename: filename
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
            } else if (list[i] === 'publickey') {
                i++;
                status = 4;
            } else if (list[i] === 'privatekey') {
                i++;
                status = 5;
            } else if (list[i] === 'signature') {
                i++;
                status = 6;
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
            } else if (status === 4) {
                ret.publickey += wtf[i];
                i++;
            } else if (status === 5) {
                ret.privatekey += wtf[i];
                i++;
            } else if (status === 6) {
                ret.signature += wtf[i];
                i++;
            }
        }

        if (ans === 'failed') {
            res.send({
                status: '403',
                msg: 'OK but encryption failed.'
            })
        }

        const options = {
            method: 'POST',
            url: 'http://127.0.0.1:6666/v1/wallet/list_keys',
            header: {'content-type': 'application/json'},
            body: JSON.stringify([Config.userName, Config.walletKey])
        };
        request(options, (error, response, body) => {
            if (error) {
                console.log(error);
                reject('Can\'t get the keys.Please checkout if you are signed in.');
                return;
            }
            try {
                console.log('wallet response: ');
                console.log(body);
                console.log('*********************************************');
                if (typeof (body) === 'string' && body[0] === '<') {
                    throw new Error('smart server error!');
                    return;
                }
                const pk = JSON.parse(body)[0][0];
                const sk = JSON.parse(body)[0][1];
                accountInfo.publicKey = pk;
                accountInfo.privateKey = sk;

                const act = {
                    account: 'admin',
                    name: 'sign',
                    authorization: [{
                        actor: msg.name,
                        permission: 'active'
                    }],
                    data: {
                        owner: msg.owner,
                        sign: pictureInfo.hash
                    }
                };

                return new Api({
                    rpc,
                    signatureProvider : new JsSignatureProvider([sk]),
                    textDecoder: new TextDecoder(),
                    textEncoder: new TextEncoder()
                }).transact({
                    actions: [act]
                }, {
                    blocksBehind: 3,
                    expireSeconds: 30,
                }).then(value => {
                    console.log(value);
                    res.send({
                        status: '200',
                        msg: 'OK successfully encrypted.',
                        strongkey: ret.strongkey,
                        weakkey: ret.weakkey,
                        publickey: ret.publickey,
                        privatekey: ret.privatekey,
                        iv: ret.iv
                    })
                }).catch(error => {
                    console.log(error);
                    res.send({
                        status: '403',
                        msg: 'OK but encryption failed.'
                    })
                });
            } catch(e) {
                console.log(e);
                res.send({
                    status: '500',
                    msg: 'something went wrong'
                })
            }
        });

    } catch(e) {
        console.log(e);
        res.send({
            status: '500',
            msg: 'something went wrong.'
        })
    }
});


module.exports = router;