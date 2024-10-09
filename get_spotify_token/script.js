const accounts_endpoint = "https://accounts.spotify.com";
const api_endpoint = "https://accounts.spotify.com/api";
const state = "";

function start_request() {
    const request_data = {
        client_id: client_id,
        response_type: "code",
        redirect_uri: encodeURI(window.location.origin + window.location.pathname),
        state: state,
        scope: "user-modify-playback-state",
    }
    const query_str = new URLSearchParams(request_data);
    let request_url = accounts_endpoint + "/authorize?" + query_str;
    window.location.href = request_url;
}

async function get_access_token() {
    const url_params = new URLSearchParams(window.location.search);
    if (url_params.has('code')) {
        const request_body = {
            grant_type: "authorization_code",
            code: url_params.get("code"),
            redirect_uri: encodeURI(window.location.origin + window.location.pathname)
        };
        body_str = new URLSearchParams(request_body);
        const response = await fetch(
            api_endpoint + "/token", 
            {
                method: "POST", 
                headers: {
                    "Authorization": "Basic " + btoa(client_id + ":" + client_secret), 
                    "Content-Type": "application/x-www-form-urlencoded"
                },
                body: body_str
            }
        );
        if (!response.ok) {
            tell_user("Error while getting tokens", response.status)
        }
        const json = await response.json();
        tell_user("Success:", JSON.stringify(json, null, 2))

    } else if (url_params.has('error')) {
        tell_user("Error while processing auth code", url_params.get('error'))
    }
}

function tell_user(title, body) {
    document.getElementById('response_title').innerHTML = title;
    document.getElementById('response_body').innerHTML = body;
}
