const router = require('express').Router();
const request = require('request');
const Config = require('../config/basic');

const { Api, JsonRpc, RpcError, Numeric } = require('eosjs');
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig');      // development only
const fetch = require('node-fetch');                                    // node only; not needed in browsers
const { TextEncoder, TextDecoder } = require('util');                   // node only; native TextEncoder/Decoder
//const { TextEncoder, TextDecoder } = require('text-encoding');          // React Native, IE11, and Edge Browsers only

const privateKeys = Config.privateKeyList;

const signatureProvider = new JsSignatureProvider(privateKeys);
const rpc = new JsonRpc('http://127.0.0.1:8000', { fetch });
const api = new Api({ rpc, signatureProvider, textDecoder: new TextDecoder(), textEncoder: new TextEncoder() });

const Ecc = require('eosjs-ecc');

router.post('/', async (req, res) => {
    try {
        const msg = {
            name: req.body.name,
            //key: req.body.key,
            IDcard : req.body.IDcard,
            email : req.body.email,
            //walletName : req.body.walletName
        };
        const accountInfo = {
            publicKey : '',
            privateKey : ''
        }
        console.log('Register: name = ' + msg.name + ', walletName = ' + msg.name);
        let walletInfo = '';
        const walletRet = {};
        const options = {
            'method' : 'POST',
            'url': 'http://127.0.0.1:6666/v1/wallet/create',
            'headers' : { 'content-type' : 'application/json'},
            'body' : '"' + msg.name + '"',
        };
        return await new Promise((resolve, reject) => {
            console.log('want to create wallet. params: ');
            console.log(options.body);
            request(options, (error, response, body) => {
                if (error) {
                    reject(error);
                    return;
                }
                //walletKey = body;
                console.log('wallet response 1:');
                console.log(body);
                if(typeof(body) === 'string') {
                    /* some problems. In body is the wallet pwd. Write it out to file.*/
                    walletInfo += 'Wallet Name : ' + msg.name + '\n';
                    walletRet.walletName = msg.name;
                    walletInfo += 'Wallet Password : ' + body + '\n';
                    walletRet.walletPassword = body;
                    resolve();
                } else {
                    console.log(body);
                    reject('Error: Cannot create wallet');
                    return;
                }
            });

        }).then(() => {
            return Ecc.randomKey();
        }).then(privateKey => {
            try {
                console.log('Private Key :\t', privateKey);
                console.log('Public Key :\t', Ecc.privateToPublic(privateKey));
                accountInfo.publicKey = Ecc.privateToPublic(privateKey);
                accountInfo.privateKey = privateKey;
                walletInfo += 'Account Name : ' + msg.name + '\n';
                walletRet.accountName = msg.name;
                walletInfo += 'Account Private Key : ' + privateKey + '\n';
                walletRet.privateKey = privateKey;
                walletInfo += 'Account Public Key : ' + Ecc.privateToPublic(privateKey) + '\n';
                walletRet.publicKey = Ecc.privateToPublic(privateKey);
                options.url = 'http://127.0.0.1:6666/v1/wallet/import_key';
                options.body = JSON.stringify([msg.name, accountInfo.privateKey]);
                options.headers = {'content-type': 'application/x-www-form-urlencoded; charset=UTF-8'}
                console.log('Importing keys. Params: ');
                console.log(options.body);
                request(options, (error, response, body) => {
                    if (error) throw new Error(error);
                    console.log('wallet response 2: ');
                    console.log(body);
                })
                console.log('Creating a new account on blockchain. Params: ');
                const act = {
                    account: 'eosio',
                    name: 'newaccount',
                    authorization: [{
                        actor: 'eosio',
                        permission: 'active',
                    }],
                    data: {
                        creator: 'eosio',
                        name: msg.name,
                        owner: {
                            threshold: 1,
                            keys: [{
                                key: Numeric.convertLegacyPublicKey(accountInfo.publicKey),
                                weight: 1
                            }],
                            accounts: [],
                            waits: []
                        },
                        active: {
                            threshold: 1,
                            keys: [{
                                key: Numeric.convertLegacyPublicKey(accountInfo.publicKey),
                                weight: 1
                            }],
                            accounts: [],
                            waits: []
                        },
                    },
                }
                console.log(JSON.stringify(act, null , 2));
                return api.transact({
                    actions: [act]
                }, {
                    blocksBehind: 3,
                    expireSeconds: 30,
                })
            } catch(e) {
                console.log(e);
                res.send({
                    status: 500,
                    message: e
                })
            }
        }).then(value => {
            console.log('blockchain response: ');
            console.log(JSON.stringify(value, null, 2));
            console.log(accountInfo.privateKey);
            const sigProvider = new JsSignatureProvider([accountInfo.privateKey]);
            const act = {
                account: 'admin',
                name: 'userregister',
                authorization: [{
                    actor: msg.name,
                    permission: 'active',
                }],
                data: {
                    uname: msg.name,
                    IDcard: msg.IDcard,
                    email: msg.email
                },
            }
            console.log('Creating new user. Params: ');
            console.log(JSON.stringify(act, null, 2));
            return new Api({ rpc,
                signatureProvider: sigProvider,
                textDecoder: new TextDecoder(),
                textEncoder: new TextEncoder()
            }).transact({
                actions : [act],
            }, {
                blocksBehind: 3,
                expireSeconds: 30,
            });
        }).then(value => {
            console.log('blockchain response2: ');
            console.log(JSON.stringify(value, null, 2));
            Config.userName = walletRet.accountName;
            Config.walletKey = walletRet.walletPassword;
            console.log('Wallet Information :\n' + walletInfo);
            res.send({
                status: 200,
                msg: "Successfully created account!",
                data: JSON.stringify(walletRet)
            });
            /*
            res.render('200', {
                status : 'success',
                message : "Successfully created account!\n" + walletInfo
            })
            */
        }).catch(error => {
            console.log(error);
            res.send({
                status: 500,
                message: error
            })
            return;
        })
        //console.log('unexpected end');

    } catch (e) {
        console.log(e);
        res.send({
            status : 500,
            message : "Something went wrong."
        });
    }
})

module.exports = router