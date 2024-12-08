import Box from "@mui/material/Box";
import {Card} from "@mui/material";
import Typography from "@mui/material/Typography";
import {Link} from "react-router";

export default function About() {
  return (
    <Box className="page center">
      <Card sx={{width: '500px', m: 'auto'}}>
        <Typography variant="h4" sx={{pb: '24px'}}>
          About
        </Typography>

        <Typography variant="body1" sx={{textAlign: 'justify'}}>
          This webapp is part of the
          <Link to="https://github.com/tiltedphoques/CyberpunkMP"> CyberpunkMP</Link> framework by
          <Link to="https://github.com/tiltedphoques"> Tilted Phoques</Link>. This web application
          is licensed under the MIT License, granting users freedom to use, modify, and distribute this software.
        </Typography>
      </Card>
    </Box>
  );
}
