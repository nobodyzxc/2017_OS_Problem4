var argName0 = "client rate";
var argName1 = "poisson";
var argName2 = "Lambda";
var initVal0 = 10;
var initVal1 = 10;
var initVal2 = 10;

$( "#slider0" ).slider({
    value: initVal0 ,
    create: function( event, ui ){
        $("#sld_head0").html(argName0 + " : " + initVal0);
    }
});
$( "#slider1" ).slider({
    value: initVal1 ,
    create: function( event, ui ){
        $("#sld_head1").html(argName1 + " : " + initVal1);
    }
});

$( "#slider2" ).slider({
    value: initVal2 ,
    create: function( event, ui ){
        $("#sld_head2").html(argName2 + " : " + initVal2);
    }
});

$( "#slider0" ).on( "slidechange", function( event, ui ) {
    $("#sld_head0").html(argName0 + " : " + ui.value);
    console.log("0 : " + ui.value);
    socket.emit('client_data' , {'name' : argName0 , 'val' :  ui.value});
});
$( "#slider1" ).on( "slidechange", function( event, ui ) {
    $("#sld_head1").html(argName1 + " : " + ui.value);
    console.log("1 : " + ui.value);
    socket.emit('client_data' , {'name' : argName1 , 'val' :  ui.value});
});
$( "#slider2" ).on( "slidechange", function( event, ui ) {
    $("#sld_head2").html(argName2 + " : " + ui.value);
    console.log("2 : " + ui.value);
    socket.emit('client_data' , {'name' : argName2 , 'val' :  ui.value});
});
