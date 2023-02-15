<?php

include'conexion.php';

if ($con) {
    echo "Conexion con base de datos exitosa! ";
    
    if(isset($_POST['temp'])) {
        $temp = $_POST['temp'];
        echo "Estación meteorológica";
        echo " temp : ".$temp;
    }
    
    if(isset($_POST['phValue'])){
        $phValue = $_POST['phValue'];
        echo " phValue : ".$phValue;
        date_default_timezone_set('america/bogota');
        $fecha_actual = date("Y-m-d H:i:s");
        
        $consulta = "INSERT INTO db_sensors(temp, phValue, fecha_actual) VALUES ('$temp','$phValue', '$fecha_actual')";
        $resultado = mysqli_query($con, $consulta);
        if ($resultado){
            echo " Registo en base de datos OK! ";
        } else {
            echo " Falla! Registro BD";
        }
    }
    
    
    
} else {
    echo "Falla! conexion con Base de datos ";   
}


?>