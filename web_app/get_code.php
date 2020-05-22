<?php
  require 'sql_pass.secret';

  if (isset($_GET['id'])) {
    $conn = mysqli_connect($sql_host, $sql_username, $sql_pass, $sql_db);
    $user_id = mysqli_real_escape_string($conn, $_GET['id']);
    $sql = "SELECT Spotify_Code FROM users_keys WHERE User_ID = '$user_id';";
    $result = mysqli_query($conn, $sql);
    if (mysqli_num_rows($result) == 1) {
      $spotify_code = mysqli_fetch_assoc($result)['Spotify_Code'];
      if (is_null($spotify_code)) {
        // The user has not authenticated yet, tell client to try again later
        echo "T";
      } else {
        // Authentication successful, return code. Single character code chosen to ease parsing in C
        echo "S$spotify_code";

        // Delete code from DB
        $delete_code_sql = "DELETE FROM users_keys WHERE User_ID = '$user_id';";
        mysqli_query($conn, $delete_code_sql);
      }
      mysqli_close($conn);
    } else {
      echo "F";
    }
  } else {
    echo "F";
  }
?>
