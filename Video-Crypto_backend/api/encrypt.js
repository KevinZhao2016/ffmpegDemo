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
    const options = {
        method: 'POST',
        url: 'http://127.0.0.1:6666/v1/wallet/list_keys',
        header: {'content-type': 'application/json'},
        body: JSON.stringify([Config.userName, Config.walletKey])
    };
    const filename = 'encrypt_' + msg.filename;
    let ret = {
        strongkey: '',
        weakkey: '',
        iv: '',
        publickey: '',
        privatekey: '',
        signature: '',
        filename: filename
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
        
        console.log('Get requests. Now Encryption...');
        return new Promise((resolve, reject) => {
            let wtf = executor('cd ' + Config.relative_path + ' && ./ffmpegDemo 1 ' + msg.filename + ' ' + filename).toString().trim();
            console.log('Encryption ended. Now split outputs...');

            let list = wtf.replace(/\r/g, '').split('\n');
            let len = list.length;
            let i = 0, status = 0, ans = 'failed';
            console.log(list);
            while(i < len) {
                if (list[i] === '') {
                    i++;
                    continue;
                }
                let j = 3 < list[i].length ? 3: list[i].length;
                if (list[i].slice(0, j) === '---') {
                    i++;
                    continue;
                }

                if (list[i] === 'strongKey') {
                    i++;
                    status = 1;
                    continue;
                } else if (list[i] === 'weakKey') {
                    i++;
                    status = 2;
                    continue;
                } else if (list[i] === 'iv'){
                    i++;
                    status = 3;
                    continue;
                } else if (list[i] === 'success') {
                    ans = 'success';
                    i++;
                    continue;
                } else if (list[i] === 'publicKey') {
                    i++;
                    status = 4;
                    continue;
                } else if (list[i] === 'privateKey') {
                    i++;
                    status = 5;
                    continue;
                } else if (list[i] === 'signature') {
                    i++;
                    status = 6;
                    continue;
                }

                if (status ===  1) {
                    ret.strongkey += list[i];
                } else if (status === 2) {
                    ret.weakkey += list[i];
                } else if (status === 3) {
                    ret.iv += list[i];
                } else if (status === 4) {
                    ret.publickey += list[i];
                } else if (status === 5) {
                    ret.privatekey += list[i];
                } else if (status === 6) {
                    ret.signature += list[i];
                    status = 0;
                }
                i++;
            }
            console.log('split finished. Now marking it on blockchains...');
            if (ans === 'failed') {
                res.send({
                    status: '403',
                    msg: 'OK but encryption failed.'
                });
                reject();
            }
            resolve();
        }).then(() => {
            return new Promise((resolve, reject) => {
                request(options, (error, response, body) => {
                    if (error) {
                        console.log(error);
                        reject('Can\'t get the keys.Please checkout if you are signed in.');
                        return;
                    }
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

                    console.log('got keys. Marking...');
                    resolve();
                });
            })
        }).then(() => {
            const act = {
                account: 'admin',
                name: 'sign',
                authorization: [{
                    actor: Config.userName,
                    permission: 'active'
                }],
                data: {
                    owner: Config.userName,
                    sign: ret.signature
                }
            };
            console.log(JSON.stringify(act,null,2));
            return new Api({
                rpc,
                signatureProvider : new JsSignatureProvider([accountInfo.privateKey]),
                textDecoder: new TextDecoder(),
                textEncoder: new TextEncoder()
            }).transact({
                actions: [act]
            }, {
                blocksBehind: 3,
                expireSeconds: 30,
            })
        }).then(value => {
            console.log(value);
            console.log("finish");
            console.log(ret);
            console.log(typeof res);
            res.send({
                status: '200',
                msg: 'OK successfully encrypted.',
                strongkey: ret.strongkey,
                weakkey: ret.weakkey,
                publickey: ret.publickey,
                privatekey: ret.privatekey,
                iv: ret.iv
            });
            console.log('resovled...');
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
            msg: 'something went wrong.'
        })
    }
});


module.exports = router;