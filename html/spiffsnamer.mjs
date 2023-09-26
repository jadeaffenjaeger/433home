import {Namer} from '@parcel/plugin';
import path from 'path';

export default new Namer({
    name({bundle}) {
        if (bundle.type === 'woff' || bundle.type === 'woff2') {
            let shortName = path.basename(bundle.getMainEntry().filePath);
            shortName = shortName.replace("lexend-peta", "lexp");
            shortName = shortName.replace("latin", "lt");
            shortName = shortName.replace("vietnamese", "vt");
            shortName = shortName.replace("-normal", "");
            return `${shortName}`;
        }

        // Allow the next namer to handle this bundle.
        return null;
    }
});
