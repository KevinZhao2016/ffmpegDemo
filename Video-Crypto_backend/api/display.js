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

/*
* display all the videos' messages from the blockchain.
* */

router.post('/', (req, res) => {
    try {
        const rows = {
            json: true,
            code: 'admin',
            table: 'video',
            scope: 'admin',
            limit: 1,
            reverse: true,
            show_payer: true
        }
        let ret = [];
        return rpc.get_table_rows(rows).then(value => {
            console.log(JSON.stringify(value, null, 2));
            const list = value.rows;
            const len = list.length;
            for( i = 0; i < len; i++) {
                ret.push({
                    ID: list[i].data.ID,
                    owner: list[i].data.owner,
                    signature: list[i].data.sign,
                    time: list[i].data.time
                })
            }
            res.send({
                status: '200',
                list: ret
            })
        })
    } catch (e) {
        console.log(e);
        res.send({
            status: '500',
            msg: 'something went wrong.'
        })
    }
});


module.exports = router;