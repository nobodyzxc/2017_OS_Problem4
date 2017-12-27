var argName0 = "cus_lambda";
var argName1 = "req_lambda";
//var argName2 = "Lambda";
var initVal0 = 17;
var initVal1 = 12;
//var initVal2 = 10;

$( "#slider0" ).slider({
    max: 30 ,
    min: 10 ,
    value: initVal0 ,
    create: function( event, ui ){
        $("#sld_head0").html(argName0 + " : " + initVal0 / 100);
    }
});
$( "#slider1" ).slider({
    max: 30 ,
    min: 10 ,
    value: initVal1 ,
    create: function( event, ui ){
        $("#sld_head1").html(argName1 + " : " + initVal1 / 100);
    }
});

//$( "#slider2" ).slider({
//    value: initVal2 ,
//    create: function( event, ui ){
//        $("#sld_head2").html(argName2 + " : " + initVal2);
//    }
//});

$( "#slider0" ).on( "slidechange", function( event, ui ) {
    $("#sld_head0").html(argName0 + " : " + ui.value / 100);
    socket.emit('client_data' , {'name' : argName0 , 'val' :  ui.value / 100});
});
$( "#slider1" ).on( "slidechange", function( event, ui ) {
    $("#sld_head1").html(argName1 + " : " + ui.value / 100);
    socket.emit('client_data' , {'name' : argName1 , 'val' :  ui.value / 100});
});
//$( "#slider2" ).on( "slidechange", function( event, ui ) {
//    $("#sld_head2").html(argName2 + " : " + ui.value);
//    console.log("2 : " + ui.value);
//    socket.emit('client_data' , {'name' : argName2 , 'val' :  ui.value});
//});
