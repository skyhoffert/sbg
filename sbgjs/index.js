
let ws = new WebSocket("ws://localhost:8001");

ws.onopen = function(e)
{
    console.log("ws opened.");
}

ws.onmessage = function(e)
{
    console.log("got msg: " + e.data);

    sbg_parse(e.data);
}

ws.onclose = function(e)
{
    console.log("ws closed.");
    
    if (e.code == 1000)
    {
        console.log("Normal closure.");
    }
    else
    {
        console.log("ERR. Unexpected closure...");
    }
}

ws.onerror = function(e)
{
    console.log("ws error.");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: index.js
// Author: Sky Hoffert
// Last Modified: 2022-08-17
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

const WIDTH = window.innerWidth;
const HEIGHT = window.innerHeight;
const BG_COLOR = 0x111111;

const PI = 3.1415926;

// Constants
///////////////////////////////////////////////////////////////////////////////////////////////////
// Pixi

const canvas = document.getElementById("canvas");
const app = new PIXI.Application({
width: WIDTH, height: HEIGHT,
backgroundColor: BG_COLOR,
resolution: window.devicePixelRatio || 1,
view: canvas,
});

PIXI.settings.SCALE_MODE = PIXI.SCALE_MODES.NEAREST; 

const content = new PIXI.Container();
const graphics = new PIXI.Graphics();
const graphics2 = new PIXI.Graphics();
app.stage.addChild(content);
content.addChild(graphics);
content.addChild(graphics2);

// Pixi
///////////////////////////////////////////////////////////////////////////////////////////////////
// Util/Math

function Linspace(a,b,d,incl=true) {
    let t = [];
    const end = incl ? b : b-d;
    for (let i = a; i <= end; i += d) {
        t.push(i);
    }
    return t;
}
function Max(ar) {
    return Math.max.apply(Math, ar);
}
function Min(ar) {
    return Math.min.apply(Math, ar);
}
function Sigs(n, dig=3) {
    return Math.round(n * Math.pow(10, dig)) / Math.pow(10, dig);
}
function RandInt(l,h) {
    return Math.floor(Math.random() * (h-l)) + l;
}
function RandID(len=6) {
    let result           = "";
    let characters       = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    let charactersLength = characters.length;
    for (let i = 0; i < len; i++ ) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    return result;
}

function RandNormal(mu, sigma, nsamples=6){
    if(!sigma) sigma = 1
    if(!mu) mu=0

    let run_total = 0
    for(let i=0 ; i<nsamples ; i++){
       run_total += Math.random()
    }

    return sigma*(run_total - nsamples/2)/(nsamples/2) + mu
}

function Cot(v) { return 1 / Math.tan(v); }
function Sin(v) { return Math.sin(v); }
function Cos(v) { return Math.cos(v); }
function Csc(v) { return 1 / Math.sin(v); }
function Ln(v) { return Math.log(v) / Math.log(Math.E); }
function Sqr(v) { return Math.pow(v,2); }
function Sqrt(v) { return Math.sqrt(v); }
function Cube(v) { return Math.pow(v,3); }
function Fourth(v) { return Math.pow(v,4); }
function Exp(v) { return Math.exp(v); }
function Log10(v) { return Math.log10(v); }
function Pow(b,e) { return Math.pow(b, e); }
function Clamp(v, min, max) {
    if (v < min) { return min; }
    if (v > max) { return max; }
    return v;
}

// Util/Math
///////////////////////////////////////////////////////////////////////////////////////////////////
// Main

let update = false;
let pause = false;

let waveform = [];

function Init() {
}

app.ticker.add((dT) => {
    if (pause) { return; }

    // graphics.clear();
    // graphics2.clear();
});

// Main
///////////////////////////////////////////////////////////////////////////////////////////////////
// Extra

function sbg_parse(s)
{
    let toks = s.split(" ");

    if (toks[0] == "dl")
    {
        console.log("wants to draw a line");

        let w = parseFloat(toks[1]);
        let c = parseInt(toks[2]);
        let ax = parseFloat(toks[3]);
        let ay = parseFloat(toks[4]);
        let bx = parseFloat(toks[5]);
        let by = parseFloat(toks[6]);

        graphics.lineStyle(w, c);
        graphics.moveTo(ax, ay);
        graphics.lineTo(bx, by);
    }
    else if (toks[0] == "gsz")
    {
        ws.send("sz " + WIDTH + " " + HEIGHT);
    }
    else
    {
        console.log("Unknown parse request.");
    }
}

// Extra
///////////////////////////////////////////////////////////////////////////////////////////////////
// Listeners

document.addEventListener("keydown", function(evt) {
    if (evt.key == "n")
    {
    }
    else if (evt.key == "b")
    {
        pause = !pause;
    }
    else if (evt.key == "m")
    {
    }
}, false);

document.addEventListener("keyup", function(evt) {
}, false);

// Listeners
///////////////////////////////////////////////////////////////////////////////////////////////////

Init();
