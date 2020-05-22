<?php
  require 'sql_pass.secret';

  if (isset($_GET['state']) && isset($_GET['code'])) {
    $conn = mysqli_connect($sql_host, $sql_username, $sql_pass, $sql_db);
    $user_id = mysqli_real_escape_string($conn,$_GET['state']);
    $spotify_code = mysqli_real_escape_string($conn, $_GET['code']);

    $set_code_sql = "UPDATE users_keys SET Spotify_Code = '$spotify_code' WHERE User_ID = '$user_id';";
    mysqli_query($conn, $set_code_sql);
    mysqli_close($conn);

    echo "The spotify authentication code will be redirected to the client now...";
  }
?>
