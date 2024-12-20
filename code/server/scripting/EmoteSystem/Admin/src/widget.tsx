import {Alert, Box, ButtonBase, Tooltip, Typography} from "@mui/material";
import React, {useEffect, useState} from "react";
// @ts-ignore
import {useToasts} from "App";
import {StatusCodes} from "http-status-codes";

// IMPORTANT: if you import an external global, you might need to use {}
//            syntax. Otherwise, import of your widget could fail to render
//            with React.
//
//            DO: import {Icon} from "@mdi/react";
//            DON'T: import Icon from "@mdi/react";

interface EmoteDto {
    readonly Username: string;
    readonly Emote: string;
}

export function Widget() {
    const showToast = useToasts();
    const [username, setUsername] = useState<string>('');
    const [emote, setEmote] = useState<string>('');

    useEffect(() => {
        return handleRefresh();
    }, []);

    // NOTE: start an interval to automatically fetch last emote every X s/m/h.

    const getLastEmote = async (): Promise<EmoteDto | undefined> => {
        const response: Response = await fetch('/api/v1/plugins/emote/');

        if (!response.ok) {
            if (response.status !== StatusCodes.NOT_FOUND) {
                showToast({
                    style: 'error',
                    message: 'Failed to request Emote API',
                });
            }
            return;
        }
        return await response.json();
    };

    const handleRefresh = () => {
        getLastEmote().then(data => {
            if (!data) {
                return;
            }
            setUsername(data.Username ?? '');
            setEmote(data.Emote ?? '');
        })
    }

    const isEmpty: boolean = username.length === 0 && emote.length === 0;

    return (
        <Tooltip title="Click to refresh">
            <ButtonBase component={Box}
                        sx={{width: '100%', height: '100%'}}
                        onClick={handleRefresh}>
                {isEmpty ?
                    <Alert severity="info">
                        No emote played for now...
                    </Alert> :
                    <Typography variant="body2" color="text.secondary">
                        Last emote was {emote} by {username}.
                    </Typography>}
            </ButtonBase>
        </Tooltip>
    );
}
