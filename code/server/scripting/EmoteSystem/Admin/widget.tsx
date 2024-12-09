import {Alert, Card, CardActionArea, Tooltip, Typography} from "@mui/material";
import {useEffect, useState} from "react";

export default function Widget() {
    const [username, setUsername] = useState<string>('');
    const [emote, setEmote] = useState<string>('');

    useEffect(() => handleRefresh(), [username, emote]);

    // NOTE: start an interval to automatically fetch last emote every X s/m/h.

    // @ts-ignore
    const getLastEmote = async () => {
        const response = await fetch('/api/v1/plugins/emote/');

        if (!response.ok) {
            return;
        }
        const json = await response.json();

        return {
            username: json.Username ?? '',
            emote: json.Emote ?? '',
        };
    };

    const handleRefresh = () => {
        // @ts-ignore
        async function request() {
            const data = await getLastEmote();

            if (lock) {
                return;
            }
            setUsername(data.username);
            setEmote(data.emote);
        }

        let lock: boolean = false;

        request();
        return () => {
            lock = true;
        };
    }

    const isEmpty = () => username.length === 0 && emote.length === 0;

    return (
        <Tooltip title="Click to refresh">
            <Card>
                <CardActionArea onClick={handleRefresh}>
                    {isEmpty() ?
                        <Alert severity="info">
                            No emote played for now...
                        </Alert> :
                        <Typography variant="body2" color="text.secondary">
                            Last emote was {emote} by {username}.
                        </Typography>}
                </CardActionArea>
            </Card>
        </Tooltip>
    );
}