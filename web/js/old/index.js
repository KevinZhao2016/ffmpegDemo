var state
let privateKeyPair = null
var render = ({
  time, days, unclaimed, today,
  eth_balance, eos_balance, publicKey, buyWindow,
})
state = Object.assign({}, state, 0)
ReactDOM.render(render(state), byId("app"))

function generate() {
    showPane('generate')
    setTimeout(() => {
        privateKeyPair = genKeyPair()
        hide("generate-progress")
        byId("generate-pubkey").innerHTML = privateKeyPair.pubkey
        byId("generate-pubkey-error").innerHTML = privateKeyPair.pubkeyError
        byId("generate-privkey").innerHTML = privateKeyPair.privkey
        byId("generate-privkey-error").innerHTML = privateKeyPair.privkeyError
        show("generate-confirm")
    })
}

function genKeyPair() {
    var {PrivateKey, PublicKey} = eos_ecc
    var d = PrivateKey.randomKey()
    var privkey = d.toWif()
    var pubkey = d.toPublic().toString()

    var pubkeyError = null
    try {
      PublicKey.fromStringOrThrow(pubkey)
    } catch(error) {
      console.log('pubkeyError', error, pubkey)
      pubkeyError = error.message + ' => ' + pubkey
    }

    var privkeyError = null
    try {
      var pub2 = PrivateKey.fromWif(privkey).toPublic().toString()
      if(pubkey !== pub2) {
        throw {message: 'public key miss-match: ' + pubkey + ' !== ' + pub2}
      }
    } catch(error) {
      console.log('privkeyError', error, privkey)
      privkeyError = error.message + ' => ' + privkey
    }

    if(privkeyError || pubkeyError) {
      privkey = 'DO NOT USE'
      pubkey = 'DO NOT USE'
    }

    return {pubkey, privkey, pubkeyError, privkeyError}
}

function showPane(name) {
    hidePanes()
    show(`${name}-pane`)
    hide(`${name}-link`)
}

function hidePanes() {
    for (var x of "generate transfer buy register".split(" ")) {
        try {
            show(`${x}-link`)
            hide(`${x}-pane`)
        } catch (error) {}
    }
}