import {Alert, Card, CardActionArea, Tooltip, Typography} from "@mui/material";
import {useEffect, useRef, useState} from "react";

interface EmoteDto {
    readonly Username: string;
    readonly Emote: string;
}

export function Widget() {
    const [username, setUsername] = useState<string>('');
    const [emote, setEmote] = useState<string>('');
    const load = useRef<boolean>(false);

    useEffect(() => {
        if (load.current) {
            return;
        }
        load.current = true;
        return handleRefresh();
    }, []);

    // NOTE: start an interval to automatically fetch last emote every X s/m/h.

    // @ts-ignore
    const getLastEmote = async (): Promise<EmoteDto | undefined> => {
        const response: Response = await fetch('/api/v1/plugins/emote/');

        if (!response.ok) {
            return;
        }
        return await response.json() as EmoteDto | undefined;
    };

    const handleRefresh = () => {
        // @ts-ignore
        async function request() {
            const data: EmoteDto | undefined = await getLastEmote();

            if (!data || lock) {
                return;
            }
            setUsername(data.Username);
            setEmote(data.Emote);
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
            <Card sx={{maxWidth: '200px'}}>
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